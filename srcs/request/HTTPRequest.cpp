#include <sstream>
#include <map>
#include <vector>

#include "HTTPRequest.hpp"
#include "split.cpp" // TODO .hpp instead of .cpp

#define LINE_END "\r\n"

const std::set<std::string> HTTPRequest::_initAcceptedMethods()
{
	std::set<std::string> methods;
	methods.insert("GET");
	methods.insert("POST");
	methods.insert("PUT");
	methods.insert("DELETE");
	methods.insert("HEAD");
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
	std::vector<std::string> requestParts = split(requestData, std::string(LINE_END), 2);

	_parseRequest(requestData);
}

void HTTPRequest::_parseRequest(std::string requestData)
{
	// try // ? TODO should it catch the exception (and set a flag?) or throw it?
	// {
	// Parse the request components
	std::vector<std::string> requestParts = split(requestData, std::string(LINE_END) + std::string(LINE_END), 2);
	if (requestParts.size() != 2)
		throw HTTPRequest::InvalidRequestException("Invalid request format");

	std::string requestHeaderSection = requestParts[0];
	std::string body = requestParts[1];

	std::vector<std::string> requestHeaderLines = split(requestHeaderSection, std::string(LINE_END), 2);
	if (requestHeaderLines.size() == 0)
		throw HTTPRequest::InvalidRequestException("Missing request line");

	std::string requestLine = requestHeaderLines[0];
	std::string headersWithoutRequestLine = "";
	if (requestHeaderLines.size() == 2)
		headersWithoutRequestLine = requestHeaderLines[1];

	// Parse the request line
	_parseRequestLine(requestLine);

	// Parse the headers
	_parseHeaders(headersWithoutRequestLine);

	// Parse the body
	_parseBody(body);

	// }
	// catch (InvalidRequestException &e)
	// {
	// 	// Handle invalid request error
	// 	std::cerr << "Error: " << e.what() << std::endl;
	// }
}

void HTTPRequest::_parseRequestLine(const std::string &requestLine) // ? TODO should return a bool instead of throwing an exception ?
{
	// ? simply use a vector instead of a string stream?
	// std::string requestLine;
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
		throw(HTTPRequest::InvalidRequestException("Invalid HTTP method '" + method + "'"));
	_requestMethod = method;
}

void HTTPRequest::_parsePath(const std::string &path)
{
	// ? TODO check if path is valid, what is a valid path?
	if (path.empty())
		throw(HTTPRequest::InvalidRequestException("Invalid path '" + path + "'"));
	_requestPath = path;
}

void HTTPRequest::_parseHttpProtocolVersion(const std::string &httpProtocolVersion)
{
	if (httpProtocolVersion.empty() || _acceptedHTTPProtocolVersions.find(httpProtocolVersion) == _acceptedHTTPProtocolVersions.end())
		throw(HTTPRequest::InvalidRequestException("Invalid HTTP protocol version '" + httpProtocolVersion + "'"));
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
	std::getline(headerLineStream, headerValue);
	if (headerValue.length() == 0)
		throw(HTTPRequest::InvalidRequestException("Invalid header value for header '" + headerName + "'"));

	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	_headers[headerName] = headerValue;
}

void HTTPRequest::_parseBody(const std::string &bodyLines)
{
	_body = bodyLines;
}

/* ****** Getters ****** */ // ! TODO best way / better way to write all getters (otehr file,..)
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
	if (_headers.find(headerName) == _headers.end())
		throw(HTTPRequest::InvalidRequestException("Header '" + headerName + "' not found"));
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
