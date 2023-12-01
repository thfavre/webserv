#include <sstream>
#include <map>
#include <vector>

#include "HTTPRequest.hpp"
#include "split.hpp"

#define LINE_END "\r\n"

const std::set<std::string> HTTPRequest::_initAcceptedMethods()
{
	std::set<std::string> methods;
	methods.insert("GET");
	methods.insert("POST");
	// methods.insert("PUT");
	methods.insert("DELETE");
	// methods.insert("HEAD");
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

HTTPRequest::HTTPRequest(const std::string &requestData)
{
	_statusCode = 0;
	std::vector<std::string> requestParts = split(requestData, std::string(LINE_END), 2);
	_parseRequest(requestData);
}

void HTTPRequest::_parseRequest(std::string requestData)
{
	try
	{
		// Parse the request components
		// printf("***Request data: ");
		// for (const char *p = requestData.c_str(); *p != '\0'; ++p)
		// {
		// 	int c = (unsigned char)*p;

		// 	switch (c)
		// 	{
		// 	case '\\':
		// 		printf("\\\\");
		// 		break;
		// 	case '\n':
		// 		printf("\\n");
		// 		break;
		// 	case '\r':
		// 		printf("\\r");
		// 		break;
		// 	case '\t':
		// 		printf("\\t");
		// 		break;

		// 		// TODO: Add other C character escapes here.  See:
		// 		// <https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences>

		// 	default:
		// 		if (isprint(c))
		// 		{
		// 			putchar(c);
		// 		}
		// 		else
		// 		{
		// 			printf("\\x%X", c);
		// 		}
		// 		break;
		// 	}
		// }
		// printf("***END\n\n");

		std::vector<std::string> requestParts = split(requestData, std::string(LINE_END) + std::string(LINE_END), 2);
		if (requestParts.size() == 1 && requestData.find("\r\n\r\n") != std::string::npos)
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
	catch (InvalidRequestException &e)
	{
		// Handle invalid request error
		if (_statusCode == 0)
			_statusCode = 400;
		std::cerr << "Error " << _statusCode << ": " << e.what() << std::endl;
	}

}

void HTTPRequest::_parseRequestLine(const std::string &requestLine) // ? TODO should return a bool instead of throwing an exception ?
{
	// ? TODO simply use a vector instead of a string stream?
	std::string requestMethod;
	std::string requestPath;
	std::string httpProtocolVersion;
	std::istringstream requestLineStream(requestLine);
	std::getline(requestLineStream, requestMethod, ' ');
	std::getline(requestLineStream, requestPath, ' ');
	std::getline(requestLineStream, httpProtocolVersion);

	_parseMethod(requestMethod);
	_parsePath(requestPath);
	_parseHttpProtocolVersion(httpProtocolVersion);
}

void HTTPRequest::_parseMethod(const std::string &method)
{
	if (method.empty() || _acceptedMethods.find(method) == _acceptedMethods.end())
	{
		_statusCode = 501;
		throw(HTTPRequest::InvalidRequestException("Invalid HTTP method '" + method + "'"));
	}
	_requestMethod = method;
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

void HTTPRequest::_parsePath(const std::string &path)
{
	if (path.empty())
	{
		_statusCode = 400;
		throw(HTTPRequest::InvalidRequestException("No path specified"));
	}
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
	// TODO put to lower case ? (because case insensitive)
	_headers[headerName] = headerValue;
}

void HTTPRequest::_parseBody(const std::string &bodyLines)
{
	_body = bodyLines;
}

void HTTPRequest::_executeMethod()
{
	if (_statusCode != 0)
	{
		// show error page
		return;
	}

	if (_requestMethod == "GET")
	{
		// show resource

	}
	else if (_requestMethod == "POST")
	{
		// create resource
	}
	else if (_requestMethod == "DELETE")
	{
		// delete resource
	}
}

bool HTTPRequest::isError() const
{
	return (_statusCode < 200 || _statusCode >= 300);
}

/* ****** Getters ****** */ // ! TODO better way to write all getters (other file,..)
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
		// throw(HTTPRequest::InvalidRequestException("Header '" + headerName + "' not found")); // ? TODO should it return an empty string instead?
		return (emptyString);
	return (_headers.at(headerName));
}

// const std::map<std::string, std::string> HTTPRequest::_getHeaders() const
// {
// 	return (_headers);
// }

const std::string &HTTPRequest::getBody() const
{
	return (_body);
}

const int &HTTPRequest::getStatusCode() const
{
	return (_statusCode);
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
