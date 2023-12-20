#include "Response.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>

// std::map<int, std::string> _statusCodes = {
// 	{200, "OK"},
// 	{201, "Created"},
// 	{202, "Accepted"},
// 	{204, "No Content"},
// 	{301, "Moved Permanently"},
// 	{302, "Found"},
// 	{304, "Not Modified"},
// 	{400, "Bad Request"},
// 	{401, "Unauthorized"},
// 	{403, "Forbidden"},
// 	{404, "Not Found"},
// 	{405, "Method Not Allowed"},
// 	{413, "Payload Too Large"},
// 	{500, "Internal Server Error"},
// 	{501, "Not Implemented"},
// 	{505, "HTTP Version Not Supported"}
// };

std::string getStatusCodeMessage(int statusCode) // ! TODO find a better way to do this (see with @fab in error class)
{
	std::map<int, std::string> _statusCodes;
	_statusCodes[200] = "OK";
	_statusCodes[201] = "Created";
	_statusCodes[202] = "Accepted";
	_statusCodes[204] = "No Content";
	_statusCodes[301] = "Moved Permanently";
	_statusCodes[302] = "Found";
	_statusCodes[304] = "Not Modified";
	_statusCodes[400] = "Bad Request";
	_statusCodes[401] = "Unauthorized";
	_statusCodes[403] = "Forbidden";
	_statusCodes[404] = "Not Found";
	_statusCodes[405] = "Method Not Allowed";
	_statusCodes[409] = "Conflict";
	_statusCodes[413] = "Payload Too Large";
	_statusCodes[500] = "Internal Server Error";
	_statusCodes[501] = "Not Implemented";
	_statusCodes[505] = "HTTP Version Not Supported";
	if (_statusCodes.find(statusCode) == _statusCodes.end())
		return ("Unknown Status Code");
	return (_statusCodes[statusCode]);
}

Response::Response(const HTTPRequest &request, int socketFd)
{
	_httpProtocolVersion = request.getHttpProtocolVersion(); // TODO variable not needed...?

	_statusCode = request.getStatusCode();
	std::string response = _formatResponse(request);
	std::cout << "**Response : \n" << std::endl << response << std::endl;
	_sendResponse(socketFd, response);
}



std::string Response::_formatResponse(const HTTPRequest &request)
{
	// set body
	std::string body;
	std::cout << "Status code : " << _statusCode << std::endl;
	if (_isError())
	{
		// // _statusMessage = "Not Found";
		// ! TODO check if the error have a custom error page
		// ! TODO if no custom error page, use default error page
		body = "<html><body><h1>Error Code : " + std::to_string(_statusCode) + " (" + getStatusCodeMessage(_statusCode) + ")" + " </h1></body></html>";
		// manageErrorResponses();
		std::cout << "Error body : " << body << std::endl;

	}
	else
	{
		body = _setBody(request);
		if (_isError())
			return _formatResponse(request);
	}

	// set headers
	std::string headers;
	headers = _setHeaders(request, body.length());

	// format response
	return headers + "\r\n" + body;
}

std::string Response::_setBody(const HTTPRequest &request)
{
	std::string body;
	std::string _root = "./"; // TODO come from config parser
	_root += request.getPath();
	std::ifstream file;
	file.open(_root.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Error opening file" << std::endl;
		_statusCode = 404; // Not Found
		return ("");
	}
	std::string line;
	while (std::getline(file, line))
		body += line;
	file.close();
	return (body);
}

std::string Response::_setHeaders(const HTTPRequest &request, int bodyLength)
{
	std::string headers;
	headers = _httpProtocolVersion + " " + std::to_string(_statusCode) + " " + getStatusCodeMessage(_statusCode) + "\r\n";
	headers += "Content-Type: text/html\r\n";
	if (bodyLength > 0)
		headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";

	// Connection
	if (request.getHeader("Connection") == "keep-alive")
		headers += "Connection: keep-alive\r\n";
	else
		headers += "Connection: close\r\n";

	return (headers);
}

// std::string Response::_getContentType(const std::string &path)
// {
// 	std::string::size_type dotIndex = path.find_last_of('.');
// 	if (dotIndex == std::string::npos)
// 		return ("application/octet-stream"); // TODO check if this is the right default value
// 	std::string extension = path.substr(dotIndex + 1);
// 	// ? TODO check if this is the right way to do this
// 	return ("text/html");
// }

void Response::_sendResponse(int socketFd, const std::string &response)
{
	// write(socketFd, _response.c_str(), _response.length());
	if (send(socketFd, response.c_str(), response.length(), MSG_DONTWAIT) != -1)
		printf("Response sent\n"); // TODO don't use printf
	else
		printf("Error sending response\n");
}

bool Response::_isError()
{
	if (_statusCode < 200 || _statusCode >= 300)
		return true;
	return false;
}

// /* ****** Getters ****** */
// const std::string Response::getResponse() const
// {
// 	return (_response);
// }