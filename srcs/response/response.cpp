#include "Response.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>

std::map<int, std::string> _statusCodes = {
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{304, "Not Modified"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{413, "Payload Too Large"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{505, "HTTP Version Not Supported"}
};

std::string getStatusCodeMessage(int statusCode)
{
	if (_statusCodes.find(statusCode) == _statusCodes.end())
		return ("Unknown Status Code");
	return (_statusCodes[statusCode]);
}

Response::Response(const HTTPRequest &request, int socketFd)
{
	_httpProtocolVersion = request.getHttpProtocolVersion();

	_statusCode = request.getStatusCode();
	_formatResponse(request);
	_sendResponse(socketFd);
}


void Response::_formatResponse(const HTTPRequest &request)
{
	// body
	std::string body;
	std::string _root = "./"; // TODO come from config parser
	_root += request.getPath();
	std::ifstream file;
	file.open(_root.c_str(),  std::ios::in);
	if (!file.is_open())
	{
		printf("Error opening file\n");
		_statusCode = 404;
		_statusMessage = "Internal Server Error";
		// _formatResponse();
		return ;
	}
	std::string line;
	while (std::getline(file, line))
		body += line;
	file.close();

	// headers
	std::string headers;
	headers = _httpProtocolVersion + " " + std::to_string(_statusCode) + " " + getStatusCodeMessage(_statusCode) + "\r\n";
	headers += "Content-Type: text/html\r\n";
	if (body.length() > 0)
		headers += "Content-Length: " + std::to_string(body.length()) + "\r\n";

	headers += "Connection: close\r\n";

	// for (auto it = _headers.begin(); it != _headers.end(); it++)
	// {
	// 	headers += it->first + ": " + it->second + "\r\n";
	// }

	_response = headers + "\r\n" + body;

}



void Response::_sendResponse(int socketFd)
{
	// write(socketFd, _response.c_str(), _response.length());
	if (send(socketFd, _response.c_str(), _response.length(), MSG_DONTWAIT) != -1)
		printf("Response sent\n"); // TODO don't use printf
	else
		printf("Error sending response\n");

}

/* ****** Getters ****** */
const std::string Response::getResponse() const
{
	return (_response);
}
