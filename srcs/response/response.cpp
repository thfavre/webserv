#include "response.hpp"
#include <fstream>

Response::Response(const HTTPRequest &request)
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

	formatResponse();
}


void Response::formatResponse()
{
	// body
	std::string body;
	// Hello World!
	// body = "<html><body><h1>Hello World!</h1></body></html>";
	std::string _root = "./test.html"; // TODO come from config parser
	std::ifstream file(_root.c_str()); // ? TODO check if file exists?
	std::string line;
	while (std::getline(file, line))
		body += line;


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
