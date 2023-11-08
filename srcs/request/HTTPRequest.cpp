#include <sstream>
#include <map>

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const std::string &requestData)
{
	_acceptedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD"}; // ? TODO where to put it (separate file,...)? // ? TODO do we only accept that?
	std::istringstream requestStream(requestData);				 // ? TODO is C++98?
	_parseRequest(requestStream);
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

void HTTPRequest::_parseRequest(std::istringstream &requestStream)
{

	// requestLine : first line of the request
	_parseRequestLine(requestStream);

	// headers :all the lines until the empty line
	_parseHeaders(requestStream);

	// body : all the line after the empty line
	_parseBody(requestStream);
}

void HTTPRequest::_parseRequestLine(std::istringstream &requestStream) // ? TODO should return a bool ?
{
	std::string requestLine;
	std::string requestMethod;
	std::string requestPath;
	std::string httpProtocolVersion;
	// TODO parse each component of the request line in a separate function
	std::getline(requestStream, requestLine);
	std::istringstream requestLineStream(requestLine);
	std::getline(requestLineStream, requestMethod, ' ');
	std::getline(requestLineStream, requestPath, ' ');
	std::getline(requestLineStream, httpProtocolVersion);

	if (requestMethod.empty() || requestPath.empty() || httpProtocolVersion.empty())
	{
		std::cerr << "Error ? requestMethod, requestPath or httpProtocolVersion does not exit" << std::endl;
		return;
	}

	// check if correct method
	if (!_isMethodValid(requestMethod))
	{
		std::cerr << "Error ? requestMethod is not valid" << std::endl;
		return;
	}
	_requestMethod = requestMethod;

	// check if correct path
	// ? TODO check if path is valid, what is a valid path?
	_requestPath = requestPath;

	// check if correct version
	if (httpProtocolVersion != "HTTP/1.0" && httpProtocolVersion != "HTTP/1.1") // ? TODO do we only accept that?
	{
		std::cerr << "Error ? httpProtocolVersion (" << httpProtocolVersion << ") is not HTTP/1.1" << std::endl;
		return;
	}
	_httpProtocolVersion = httpProtocolVersion;



}

void HTTPRequest::_parseHeaders(std::istringstream &requestStream)
{
	std::string currentHeaderLine;
	while (std::getline(requestStream, currentHeaderLine) && !currentHeaderLine.empty())
		_parseHeaderLine(currentHeaderLine);
}

void HTTPRequest::_parseHeaderLine(const std::string &headerLine)
{
	std::istringstream headerLineStream(headerLine);

	std::string headerName;
	std::getline(headerLineStream, headerName, ':');

	std::string headerValue;
	std::getline(headerLineStream, headerValue);

	if (headerValue.length() == 0)
	{
		std::cerr << "Error ? " << headerValue << " of " << headerName << " does not exit" << std::endl;
		return;
	}

	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	_headers[headerName] = headerValue;
}

void HTTPRequest::_parseBody(std::istringstream &requestStream)
{
	std::getline(requestStream, _body, '\0');
}


bool HTTPRequest::_isMethodValid(const std::string& method) const
{
	return (_acceptedMethods.find(method) != _acceptedMethods.end());
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
	return (_headers.at(headerName));
}

const std::map<std::string, std::string> HTTPRequest::_getHeaders() const
{
	return (_headers);
}

const std::string &HTTPRequest::getBody() const
{
	return (_body);
}
