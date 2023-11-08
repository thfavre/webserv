#include <sstream>
#include <map>
#include <vector>

#include "HTTPRequest.hpp"

// ! TODO put somewhere else
template <typename T>
std::vector<T> split(const std::string& str, const T& delimiter) {
  std::vector<T> tokens;
  std::size_t start = 0;
  std::size_t pos = 0;

  while ((pos = str.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, pos - start));
    start = pos + delimiter.length();
  }

  if (start < str.length()) {
    tokens.push_back(str.substr(start));
  }

  return tokens;
}

// max is the maximum number of splits
template <typename T>
std::vector<T> split(const std::string& str, const T& delimiter, int max) {
  std::vector<T> tokens;
  std::size_t start = 0;
  std::size_t pos = 0;
  int splits = 0;

  while ((pos = str.find(delimiter, start)) != std::string::npos) {
    if (splits < max - 1) {
      tokens.push_back(str.substr(start, pos - start));
      splits++;
      start = pos + delimiter.length();
    } else {
      break;
    }
  }

  if (start < str.length()) {
    tokens.push_back(str.substr(start));
  }

  return tokens;
}

#define LINE_END "\r\n"

HTTPRequest::HTTPRequest(const std::string &requestData)
{
	_acceptedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD"}; // ? TODO where to put it (separate file, public static?...)? // ? TODO do we only accept that?
	_parseRequest(requestData);
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

void HTTPRequest::_parseRequest(std::string requestData) {
  try {
    // Parse the request components
    std::vector<std::string> requestParts = split(requestData, std::string(LINE_END) + std::string(LINE_END), 2);
    if (requestParts.size() != 2) {
      throw HTTPRequest::InvalidRequestException("Invalid request format");
    }

    std::string requestHeaderSection = requestParts[0];
    std::string body = requestParts[1];

    std::vector<std::string> requestHeaderLines = split(requestHeaderSection, std::string(LINE_END), 2);
    if (requestHeaderLines.size() == 0) {
      throw HTTPRequest::InvalidRequestException("Missing request line");
    }

    std::string requestLine = requestHeaderLines[0];
    std::string headersWithoutRequestLine = "";
    if (requestHeaderLines.size() == 2) {
      headersWithoutRequestLine = requestHeaderLines[1];
    }

    // Parse the request line
    _parseRequestLine(requestLine);

    // Parse the headers
    _parseHeaders(headersWithoutRequestLine);

    // Parse the body
    _parseBody(body);
  } catch (InvalidRequestException& e) {
    // Handle invalid request error
	std::cerr << "Error: " << e.what() << std::endl;
  }
}



void HTTPRequest::_parseRequestLine(const std::string& requestLine) // ? TODO should return a bool ?
{
	// std::string requestLine;
	std::string requestMethod;
	std::string requestPath;
	std::string httpProtocolVersion;
	// TODO parse each component of the request line in a separate function
	std::istringstream requestLineStream(requestLine);
	std::getline(requestLineStream, requestMethod, ' ');
	std::getline(requestLineStream, requestPath, ' ');
	std::getline(requestLineStream, httpProtocolVersion);

	if (requestMethod.empty() || requestPath.empty() || httpProtocolVersion.empty())
	{
		// std::cerr << "Error ? requestMethod, requestPath or httpProtocolVersion does not exit" << std::endl;
		return;
	}

	// check if correct method
	if (!_isMethodValid(requestMethod))
	{
		// std::cerr << "Error ? requestMethod is not valid" << std::endl;
		return;
	}
	_requestMethod = requestMethod;

	// check if correct path
	// ? TODO check if path is valid, what is a valid path?
	_requestPath = requestPath;

	// check if correct version
	if (httpProtocolVersion != "HTTP/1.0" && httpProtocolVersion != "HTTP/1.1") // ? TODO do we only accept that?
	{
		// std::cerr << "Error ? httpProtocolVersion (" << httpProtocolVersion << ") is not HTTP/1.1" << std::endl;
		return;
	}
	_httpProtocolVersion = httpProtocolVersion;



}

void HTTPRequest::_parseHeaders(const std::string& headersLines)
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
	std::getline(headerLineStream, headerName, ':');

	std::string headerValue;
	std::getline(headerLineStream, headerValue);

	if (headerValue.length() == 0)
	{
		// std::cerr << "Error ? " << headerValue << " of " << headerName << " does not exit" << std::endl;
		return;
	}

	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	_headers[headerName] = headerValue;
}

void HTTPRequest::_parseBody(const std::string& bodyLines)
{
	_body = bodyLines;
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
