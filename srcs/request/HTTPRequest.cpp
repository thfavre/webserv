#include <sstream>
#include <fstream>
#include <map>
#include <vector>

#include "HTTPRequest.hpp"
#include "CGIHandler.hpp"
#include "split.hpp"
#include "checkFileExists.hpp"

#include "../colors.hpp"



const std::set<std::string> HTTPRequest::_initAcceptedMethods()
{
	std::set<std::string> methods;
	methods.insert("GET");
	methods.insert("POST");
	methods.insert("DELETE");
	return (methods);
}

const std::set<std::string> HTTPRequest::_acceptedMethods = HTTPRequest::_initAcceptedMethods();

const std::set<std::string> HTTPRequest::_initAcceptedHTTPProtocolVersions()
{
	std::set<std::string> httpProtocolVersions;
	httpProtocolVersions.insert("HTTP/1.0");
	httpProtocolVersions.insert("HTTP/1.1");
	return (httpProtocolVersions);
}

const std::set<std::string> HTTPRequest::_acceptedHTTPProtocolVersions = HTTPRequest::_initAcceptedHTTPProtocolVersions();

HTTPRequest::HTTPRequest(const std::string &requestData, const t_server &server) : _statusCode(0), _server(server), _isCGI(false)
{
	try
	{
		_parseRequest(requestData);
		if (_statusCode != 0)
			return;
		_statusCode = 200;
		if (CGIHandler(_requestPath).isCGI())
		{
			if (_configRootOptions.find("cgi") != _configRootOptions.end())
			{
				std::vector<std::string> cgi = split(_configRootOptions["cgi"], " ");
				if (cgi.size() != 2)
				{
					_statusCode = 500;
					throw HTTPRequest::InvalidRequestException("Invalid CGI config");
				}
				if (cgi[0][0] == '.')
					cgi[0].erase(0, 1);
				if (cgi[0] == CGIHandler(_requestPath).getExtension())
				{
					_CGIPath = cgi[1];
					_isCGI = true;
					std::cout << LOG_COLOR << "[LOG] The file is a CGI" << RESET << " (path: " << _CGIPath << ")" << std::endl;
				}
				else
				{
					_statusCode = 403;
					throw HTTPRequest::InvalidRequestException("The CGI need a " + cgi[0] + " extension");
				}
			}
			else
			{
				_statusCode = 403;
				throw HTTPRequest::InvalidRequestException("CGI is not allowed by the server config");
			}
		}
		else
			_executeMethod();
	}
	catch (InvalidRequestException &e)
	{
		if (_statusCode == 0)
			_statusCode = 400;
		std::cout << RED << "[ERROR] " << _statusCode << ": " << RESET << e.what() << std::endl;
	}
}

void HTTPRequest::_parseRequest(std::string requestData)
{
	std::cout << LOG_COLOR << "[LOG] Rquest " << RESET << "(" << requestData.length() << " bytes)" << std::endl;
	if (requestData.length() > 1000)
		std::cout << LOG_COLOR2 << "(Do only contains the 1000 first bytes)" << RESET << std::endl;
	std::cout << requestData.substr(0, 1000) << std::endl;
	std::cout << LOG_COLOR << "[LOG] End of request" << RESET << std::endl;

	std::vector<std::string> requestParts = split(requestData, std::string(LINE_END) + std::string(LINE_END), 2);
	if (requestParts.size() == 1 && requestData.find(std::string(LINE_END) + std::string(LINE_END)) != std::string::npos)
		requestParts.push_back("");
	if (requestParts.size() != 2)
	{
		_statusCode = 400;
		throw HTTPRequest::InvalidRequestException("Invalid request format");
	}

	std::string requestHeaderSection = requestParts[0];
	std::string body = requestParts[1];

	std::vector<std::string> requestHeaderLines = split(requestHeaderSection, std::string(LINE_END), 2);
	if (requestHeaderLines.size() == 0)
	{
		_statusCode = 400;
		throw HTTPRequest::InvalidRequestException("Missing request line");
	}
	std::string requestLine = requestHeaderLines[0];
	std::string headersWithoutRequestLine = "";
	if (requestHeaderLines.size() == 2)
		headersWithoutRequestLine = requestHeaderLines[1];

	_parseRequestLine(requestLine);
	_parseHeaders(headersWithoutRequestLine);
	_parseBody(body);
}

void HTTPRequest::_parseRequestLine(const std::string &requestLine)
{
	std::string requestMethod;
	std::string requestPath;
	std::string httpProtocolVersion;
	std::istringstream requestLineStream(requestLine);
	std::getline(requestLineStream, requestMethod, ' ');
	std::getline(requestLineStream, requestPath, ' ');
	std::getline(requestLineStream, httpProtocolVersion);

	_getConfigRootOptions(requestPath);
	_parseHttpProtocolVersion(httpProtocolVersion);
	_parseMethod(requestMethod);
	_parsePath(requestPath);
}

void HTTPRequest::_parseMethod(const std::string &method)
{
	if (method.empty() || _acceptedMethods.find(method) == _acceptedMethods.end())
	{
		_statusCode = 501;
		throw(HTTPRequest::InvalidRequestException("Invalid HTTP method '" + method + "'"));
	}

	if (_configRootOptions.find("methods") != _configRootOptions.end())
	{
		std::vector<std::string> methods = split(_configRootOptions["methods"], " ");
		if (std::find(methods.begin(), methods.end(), method) != methods.end())
		{
			_requestMethod = method;
			return;
		}
	}
	_statusCode = 405;
	throw(HTTPRequest::InvalidRequestException("Method '" + method + "'" + " is not allowed by the server config"));
}

bool HTTPRequest::_areAllPathCharactersValid(const std::string &path)
{
	const std::string allowedSpecialChars = "_-.~:/?#[]@!$&'()*+,;=";
	for (size_t i = 0; i < path.length(); i++)
	{
		if (!isalnum(path[i]) && allowedSpecialChars.find(path[i]) == std::string::npos)
			return false;
	}
	return true;
}

bool HTTPRequest::_isSafePath(const std::string &path)
{
	// Check for directory traversal
	return path.find("..") == std::string::npos;
}

bool HTTPRequest::_isPathLengthValid(const std::string &path, size_t maxLength)
{
	return path.length() <= maxLength;
}

void HTTPRequest::_parsePath(std::string path)
{
	if (path.length() > 1 && path[path.length() - 1] == '/')
		path.resize(path.length() - 1);
	path = _getRedirectedPath(path);

	if (!_areAllPathCharactersValid(path))
	{
		_statusCode = 400;
		throw HTTPRequest::InvalidRequestException("Invalid path '" + path + "'");
	}
	if (!_isSafePath(path))
	{
		_statusCode = 403;
		throw HTTPRequest::InvalidRequestException("Invalid path '" + path + "'");
	}
	if (!_isPathLengthValid(path, MAX_PATH_LENGTH))
	{
		_statusCode = 414;
		throw HTTPRequest::InvalidRequestException("Path '" + path + "' too long");
	}
	_requestPath = path;
}

void HTTPRequest::_getConfigRootOptions(const std::string &requestPath)
{
	std::string path = requestPath;
	std::map<std::string, std::string> options;
	while (path != "")
	{
		for (std::map<std::string, std::map<std::string, std::string> >::const_iterator route = _server.routes.begin();
			 route != _server.routes.end(); ++route)
		{
			if (path == route->first)
			{
				options = route->second;
				std::cout << LOG_COLOR << "[LOG] Route found with options: " << RESET << std::endl;
				for (std::map<std::string, std::string>::const_iterator option = options.begin();
					 option != options.end(); ++option)
				{
					std::cout << LOG_COLOR2 << "\t" << option->first << ": " << RESET << option->second << std::endl;
				}
				_configRootOptions = options;
				_configRoute = path;

				return ;
			}
		}
		if (path == "/")
		{
			_statusCode = 400;
			throw HTTPRequest::InvalidRequestException("Path (or subpath) is not in config");
		}
		path = path.substr(0, path.find_last_of('/'));
		if (path.length() == 0)
			path = "/";
	}
	_statusCode = 400;
	throw HTTPRequest::InvalidRequestException("Path or (or subpath) is not in config");
}

const std::string HTTPRequest::_getRedirectedPath(const std::string &path)
{
	std::string redirection = path;
	if (path == _configRoute)
	{
		if (_configRootOptions.find("redirection") != _configRootOptions.end())
		{
			redirection = _configRootOptions["redirection"];
			_statusCode = 301;

			std::cout << LOG_COLOR << "[LOG] The path is redirected to: " << RESET << redirection << std::endl;
			return (redirection);
		}
		else if (_configRootOptions.find("index") != _configRootOptions.end())
		{
			redirection = "/" + _configRootOptions["index"];
			std::cout << LOG_COLOR << "[LOG] Index found, the path is redirected to: " << RESET << redirection << std::endl;
			return (redirection);
		}
	}
	redirection = path.substr(_configRoute.length());
	if (redirection == "")
			redirection = "/";
	else if (redirection[0] != '/')
		redirection = "/" + redirection;

	return (redirection);
}

void HTTPRequest::_parseHttpProtocolVersion(const std::string &httpProtocolVersion)
{
	if (httpProtocolVersion.empty() || _acceptedHTTPProtocolVersions.find(httpProtocolVersion) == _acceptedHTTPProtocolVersions.end())
	{
		_statusCode = 505;
		throw(HTTPRequest::InvalidRequestException("Invalid HTTP protocol version '" + httpProtocolVersion + "'"));
	}
	_httpProtocolVersion = httpProtocolVersion;
}

void HTTPRequest::_parseHeaders(const std::string &headersLines)
{
	std::vector<std::string> headersLinesVector = split(headersLines, std::string(LINE_END));
	for (std::vector<std::string>::iterator headerLine = headersLinesVector.begin();
		 headerLine != headersLinesVector.end(); ++headerLine)
	{
		_parseHeaderLine(*headerLine);
	}
}

void HTTPRequest::_parseHeaderLine(const std::string &headerLine)
{
	std::istringstream headerLineStream(headerLine);
	std::string headerName;
	std::string headerValue;

	std::getline(headerLineStream, headerName, ':');
	if (headerName.length() == 0)
	{
		_statusCode = 400;
		throw(HTTPRequest::InvalidRequestException("Invalid header name"));
	}
	std::getline(headerLineStream, headerValue);
	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	if (headerValue.length() == 0)
	{
		_statusCode = 400;
		throw(HTTPRequest::InvalidRequestException("Invalid header value for header '" + headerName + "'"));
	}
	_headers[headerName] = headerValue;
}

void HTTPRequest::_parseBody(const std::string &bodyLines)
{
	if (bodyLines.length() > _server.client_max_body_size)
	{
		_statusCode = 413;
		throw HTTPRequest::InvalidRequestException("Body too long");
	}
	if (_headers.find("Content-Type") != _headers.end())
	{
		if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
			_parseMultiPartBody(bodyLines);
		else if (_headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
		{
			_parseUrlencodedBody(bodyLines);
		}
		else
			std::cout << LOG_COLOR << "[LOG] " << RESET << "Content-Type not supported" << std::endl;
	}
	_body = bodyLines;
}

void HTTPRequest::_parseMultiPartBody(const std::string &bodyLines)
{
	std::cout << LOG_COLOR << "[LOG] " << RESET << "Content-Type: multipart/form-data" << std::endl;
	std::string boundary = _getMultiPartBoundary();

	std::size_t start = bodyLines.find(boundary);
	if (start == std::string::npos)
	{
		_statusCode = 400; // Bad Request
		throw HTTPRequest::InvalidRequestException("boundary start not found in body");
	}
	std::size_t end = bodyLines.find(boundary+"--", start + boundary.length());
	if (end == std::string::npos)
	{
		_statusCode = 400; // Bad Request
		throw HTTPRequest::InvalidRequestException("boundary end not found in body");
	}
	std::string headers_and_body = bodyLines.substr(start + boundary.length(), end - start - boundary.length());
	std::string headers = headers_and_body.substr(0, headers_and_body.find(std::string(LINE_END) + std::string(LINE_END)));
	std::string body = headers_and_body.substr(headers_and_body.find(std::string(LINE_END) + std::string(LINE_END)) + 4);
	std::size_t filename_start = headers.find("filename=");
	if (filename_start == std::string::npos)
	{
		_statusCode = 400; // Bad Request
		throw HTTPRequest::InvalidRequestException("filename not found in Content-Disposition header");
	}
	std::size_t filename_end = headers.find('"', filename_start + 10);
	_post_file_name = headers.substr(filename_start + 10, filename_end - filename_start - 10);

	_post_file_content = body.substr(0, body.length() - 2);
}

std::string HTTPRequest::_getMultiPartBoundary()
{
	if (_headers.find("Content-Type") == _headers.end())
	{
		_statusCode = 400; // Bad Request
		throw HTTPRequest::InvalidRequestException("Content-Type header not found");
	}
	std::string content_type = _headers.find("Content-Type")->second;
	std::size_t start = content_type.find("boundary=");
	if (start == std::string::npos)
	{
		_statusCode = 400; // Bad Request
		throw HTTPRequest::InvalidRequestException("boundary not found in Content-Type header");
	}
	return ("--" + content_type.substr(start + 9));
}

void HTTPRequest::_parseUrlencodedBody(const std::string &bodyLines)
{
	std::cout << LOG_COLOR << "[LOG] " << RESET << "Content-Type: application/x-www-form-urlencoded" << std::endl;
	std::cout << LOG_COLOR << "\t[LOG] Parameters: " << RESET  << std::endl;
	std::vector<std::string> parameters = split(bodyLines, "&");
	for (std::vector<std::string>::iterator parameter = parameters.begin();
		 parameter != parameters.end(); ++parameter)
	{
		std::vector<std::string> parameterParts = split(*parameter, "=");
		if (parameterParts.size() == 2)
		{
			std::cout << "\t\t" << parameterParts[0] << ": " << parameterParts[1] << std::endl;
			_urlParameters.push_back(parameterParts[1]);
		}
	}
}


void HTTPRequest::_executeMethod()
{
	if (_requestMethod == "GET")
	{
	}
	else if (_requestMethod == "POST")
	{
		std::string upload_path = UPLOAD_FOLDER + _post_file_name;
		if (checkFileExists(upload_path))
		{
			_statusCode = 409; // Conflict
			return;
		}

		std::ofstream file;
		std::cout << _post_file_name << std::endl;
		file.open(upload_path, std::ios::out);

		if (!file.is_open())
		{
			_statusCode = 500; // Internal Server Error
			throw HTTPRequest::InvalidRequestException("Error creating file");
			return;
		}
		file << _post_file_content;
		file.close();
		_statusCode = 201; // Created
	}
	else if (_requestMethod == "DELETE")
	{
		std::string path = getRoot() + _requestPath;
		if (!checkFileExists(path))
		{
			_statusCode = 404; // Not Found
			throw HTTPRequest::InvalidRequestException("File does not exist");
		}
		if (remove(path.c_str()) != 0)
		{
			_statusCode = 500; // Internal Server Error
			return;
		}
		_statusCode = 200; // OK
	}
}

const std::string &HTTPRequest::getMethod() const
{
	return (_requestMethod);
}

const std::string &HTTPRequest::getPath() const
{
	return (_requestPath);
}

const std::string &HTTPRequest::getHttpProtocolVersion() const
{
	return (_httpProtocolVersion);
}

const std::string &HTTPRequest::getHeader(const std::string &headerName) const
{
	static const std::string emptyString = "";
	if (_headers.find(headerName) == _headers.end())
		return (emptyString);
	return (_headers.at(headerName));
}

const std::string &HTTPRequest::getBody() const
{
	return (_body);
}

const int &HTTPRequest::getStatusCode() const
{
	return (_statusCode);
}

const std::string &HTTPRequest::getRoot() const
{
	if (_configRootOptions.find("root") == _configRootOptions.end())
	{
		static const std::string noRootRoot = "./";
		return (noRootRoot);
	}
	const std::string &root = _configRootOptions.at("root");
	return (root);
}

bool HTTPRequest::getReperoryListing() const
{
	if (_configRootOptions.find("repertory_listing") == _configRootOptions.end())
		return (false);
	if (_configRootOptions.at("repertory_listing") == "true" || _configRootOptions.at("repertory_listing") == "on")
		return (true);
	return (false);
}

bool HTTPRequest::isCGI() const
{
	return (_isCGI);
}

const std::string HTTPRequest::getCGIPath() const
{
	return (_CGIPath);
}

const std::list<std::string> &HTTPRequest::getUrlParameters() const
{
	return (_urlParameters);
}

std::ostream &operator<<(std::ostream &stream, const HTTPRequest &request)
{
	// request line
	stream << "Method: " << request.getMethod() << std::endl;
	stream << "Path: " << request.getPath() << std::endl;
	stream << "HTTP Protocol Version: " << request.getHttpProtocolVersion() << std::endl;
	// headers
	stream << "Headers: " << std::endl
		   << "{" << std::endl;
	for (std::map<std::string, std::string>::const_iterator header = request._headers.begin();
		 header != request._headers.end(); ++header)
	{
		stream << "    " << header->first << ": " << header->second << std::endl;
	}
	stream << "}" << std::endl;
	// body
	stream << "Body: " << std::endl
		   << request.getBody() << std::endl;
	return (stream);
}

void	HTTPRequest::setStatusCode(int code)
{
	_statusCode = code;
}
