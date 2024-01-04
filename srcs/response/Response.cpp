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
	_statusCodes[508] = "Infinite Loop Detected";
	if (_statusCodes.find(statusCode) == _statusCodes.end())
		return ("Unknown Status Code");
	return (_statusCodes[statusCode]);
}

Response::Response(const HTTPRequest &request, int socketFd, const t_server &server) : _server(server)
{
	_httpProtocolVersion = request.getHttpProtocolVersion(); // TODO variable not needed...?

	_statusCode = request.getStatusCode();
	// if is cgi
	// formart cgi response (read the output of the cgi from a pipe)
	// else
	std::string response = _formatResponse(request);
	std::cout << "**Response : \n"
			  << std::endl
			  << response << std::endl;
	_sendResponse(socketFd, response);
}

std::string Response::_formatResponse(const HTTPRequest &request)
{
	// set body
	std::string body;
	std::cout << "Status code : " << _statusCode << std::endl;
	if (_isError())
	{
		// TODO ? put in a function
		// // _statusMessage = "Not Found";
		// ! TODO check if the error have a custom error page
		// check in the config if there is a custom error page for this error
		if (_server.error_pages.find(_statusCode) != _server.error_pages.end())
		{
			std::cout << "Custom error page found" << std::endl;
			std::string errorPagePath = _server.error_pages[_statusCode];
			std::ifstream file;
			file.open(errorPagePath.c_str(), std::ios::in);
			if (!file.is_open())
			{
				std::cerr << "Error opening file '"<< errorPagePath<< "'" << std::endl;
				_statusCode = 404; // Not Found
				body = _formatGenericErrorPageHTML();
			}
			std::string line;
			while (std::getline(file, line))
				body += line;
			file.close();
		}
		// ! TODO if no custom error page, use default error page
		else
			body = _formatGenericErrorPageHTML();
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

std::string Response::_formatGenericErrorPageHTML()
{
	std::string body;
	body += "<html><body><h1>Error " + std::to_string(_statusCode) + "</h1></body></html>";
	body += "<p>" + getStatusCodeMessage(_statusCode) + "</p>";
	return (body);
}

std::string Response::_setBody(const HTTPRequest &request)
{
	std::string body;

	// if cgi
	if (request.isCGI())
	{
		CGIHandler cgiHandler = CGIHandler(request.getPath());
		if (cgiHandler.executeScript(request.getCGIPath()))
		{
			_statusCode = 200; // OK
			return (cgiHandler.getScriptExecutionOutput());
		}
		else if (cgiHandler.isInfLoop())
		{
			_statusCode = 508; // Internal Server Error
			return ("");
		}
		else
		{
			_statusCode = 500; // Internal Server Error
			return ("");
		}
	}

	else
	{
		std::string path = request.getPath();
		std::cout << "path : " << path << std::endl;
		std::ifstream file;
		file.open(path.c_str(), std::ios::in);
		if (!file.is_open())
		{
			std::cerr << "Error opening file '" << path<< "'" << std::endl;
			_statusCode = 404; // Not Found
			return ("");
		}
		std::string line;
		while (std::getline(file, line))
			body += line;
		file.close();
		return (body);
	}
}

std::string Response::_setHeaders(const HTTPRequest &request, int bodyLength)
{
	std::string headers;

	headers = _httpProtocolVersion + " " + std::to_string(_statusCode) + " " + getStatusCodeMessage(_statusCode) + "\r\n";
	headers += "Content-Type: text/html\r\n"; // TODO put the right content type
	// if (bodyLength > 0)
	headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";

	// Connection
	if (request.getHeader("Connection") == "close")
		headers += "Connection: close\r\n";
	else // TODO or only close if Connection: close in request ?
		headers += "Connection: keep-alive\r\n";
	// Location
	if (_statusCode == 301 || _statusCode == 302)
		headers += "Location: " + request.getPath() + "\r\n";
	// TODO

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
	if (send(socketFd, response.c_str(), response.length(), MSG_DONTWAIT) != -1)
	{
		std::cout << "Response sent" << std::endl;
		// if connexion close -> close socket ?
	}
	else
		std::cerr << "Error sending response" << std::endl;
}

bool Response::_isError()
{
	if (_statusCode < 200 || _statusCode > 301) // 301 is not an error
		return true;
	return false;
}

// /* ****** Getters ****** */
// const std::string Response::getResponse() const
// {
// 	return (_response);
// }
