#include "Response.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>


Response::Response(const HTTPRequest &request, int socketFd)
{
	_httpProtocolVersion = request.getHttpProtocolVersion();

	_statusCode = request.getStatusCode();
	if (_statusCode == 200)
		_statusMessage = "OK";
	else if (_statusCode == 404)
		_statusMessage = "Not Found";
	else if (_statusCode == 500)
		_statusMessage = "Internal Server Error";
	else
		_statusMessage = "Unknown Status Code";

	// _statusCode = "200";
	// _statusMessage = "OK";
	// _headers["Content-Type"] = "text/html";

	_formatResponse(request);
	_sendResponse(socketFd);
}


void Response::_formatResponse(const HTTPRequest &request)
{
	// body
	std::string body;
	// Hello World!
	// body = "<html><body><h1>Hello World!</h1></body></html>";
	std::string _root = "./"; // TODO come from config parser
	_root += request.getPath();
	std::ifstream file;//(_root.c_str(),  std::ios::in); // ? TODO check if file exists?
	file.open(_root.c_str(),  std::ios::in);
	if (!file.is_open())
	{
		printf("Error opening file\n");
		_statusCode = 500;
		_statusMessage = "Internal Server Error";
		// _formatResponse();
		return ;
	}
	std::string line;
	while (std::getline(file, line)) // use the >> operator instead?
		body += line;
	file.close();

	// headers
	std::string headers;
	headers = _httpProtocolVersion + " " + std::to_string(_statusCode) + " " + _statusMessage + "\r\n";
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
	send(socketFd, _response.c_str(), _response.length(), MSG_DONTWAIT);
}

/* ****** Getters ****** */
const std::string Response::getResponse() const
{
	return (_response);
}
