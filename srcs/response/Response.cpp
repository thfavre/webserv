#include "Response.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>

#include "../colors.hpp"

std::string getStatusCodeMessage(int statusCode)
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
	(void) socketFd;
	_httpProtocolVersion = request.getHttpProtocolVersion();
	_statusCode = request.getStatusCode();
	std::string response = _formatResponse(request);
	this->_response = response;

	// Log infos
	std::cout << LOG_COLOR << "[LOG]" << " Response " << RESET << "(" << response.length() << " bytes):" << std::endl;
	if (response.length() > 1000)
		std::cout << LOG_COLOR2 << "(Do only contains the 1000 first bytes)" << RESET << std::endl;
	std::cout << response.substr(0, 1000) << std::endl;
	std::cout << LOG_COLOR << "[LOG] End of response" << RESET << std::endl;
}

std::string Response::_formatResponse(const HTTPRequest &request)
{
	_contentType = _getContentType(request.getPath());
	// set body
	std::string body;
	std::cout << LOG_COLOR << "[LOG]" << " Response status code : " << RESET << _statusCode << std::endl;
	if (_isError())
	{
		_contentType = "text/html";
		if (_server.error_pages.find(_statusCode) != _server.error_pages.end())
		{
			std::cout << LOG_COLOR << "[LOG]" << " Custom error page found" << RESET << std::endl;
			std::string errorPagePath = _server.error_pages[_statusCode];
			std::ifstream file;
			file.open(errorPagePath.c_str(), std::ios::in);
			if (!file.is_open())
			{
				std::cout << ERR_COLOR << "[ERR]" << " Error opening file '" << RESET << errorPagePath << ERR_COLOR << "'" << RESET << std::endl;
				_statusCode = 404; // Not Found
				body = _formatGenericErrorPageHTML();
			}
			std::string line;
			while (std::getline(file, line))
				body += line;
			file.close();
		}
		else
			body = _formatGenericErrorPageHTML();
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

std::string Response::_getContentType(const std::string &path)
{
	std::string::size_type dotIndex = path.find_last_of('.');
	if (dotIndex == std::string::npos)
		return ("application/octet-stream");
	std::string extension = path.substr(dotIndex + 1);
	if (extension == "html")
		return ("text/html");
	else if (extension == "css")
		return ("text/css");
	else if (extension == "js")
		return ("text/javascript");
	else if (extension == "jpg")
		return ("text/jpeg");
	else if (extension == "jpeg")
		return ("text/jpeg");
	else if (extension == "png")
		return ("text/png");
	else if (extension == "gif")
		return ("text/gif");
	else if (extension == "svg")
		return ("text/svg+xml");
	else if (extension == "ico")
		return ("text/x-icon");
	else if (extension == "mp3")
		return ("audio/mpeg");
	else if (extension == "mp4")
		return ("video/mp4");
	else if (extension == "ttf")
		return ("font/ttf");
	else if (extension == "json")
		return ("application/json");
	else if (extension == "pdf")
		return ("application/pdf");
	else if (extension == "xml")
		return ("application/xml");
	else
		return ("application/octet-stream");
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
	if (_statusCode == 301)
		return "";

	std::string path = request.getRoot() + request.getPath();
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR) // The path is a directory
		{
			std::cout << LOG_COLOR << "[LOG]" << " The path is a directory" << RESET << std::endl;
			return (_setDirectoryBody(path, request.getReperoryListing(), request.getRoot().length()));
		}
		else if (s.st_mode & S_IFREG) // The path is a file
		{
			std::cout << LOG_COLOR << "[LOG]" << " The path is a file" << RESET << std::endl;
			if (request.isCGI())
				return (_setCGIBody(path, request.getCGIPath(), request.getUrlParameters()));
			return (_setFileBody(path));
		}
		else // The path is not a directory nor a file
		{
			std::cout << ERR_COLOR << "[ERROR]" << " The path is not a directory nor a file" << RESET << std::endl;
			_statusCode = 404;
			return ("");
		}
	}
	_statusCode = 404;
	return ("");
}

std::string Response::_setCGIBody(const std::string &path, const std::string &CGIPath, const std::list<std::string> &args)
{
		_contentType = "text/html";
		CGIHandler cgiHandler = CGIHandler(path, args);
		if (cgiHandler.executeScript(CGIPath))
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


std::string Response::_setFileBody(const std::string &path)
{
	std::ifstream file;

	file.open(path.c_str());
	if (!file.is_open())
	{
		std::cout << ERR_COLOR << "[ERR]" << " Error opening file '" << RESET << path << ERR_COLOR << "'" << RESET << std::endl;
		_statusCode = 404; // Not Found
		return ("");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string body = buffer.str();
	return (body);

}

std::string Response::_setDirectoryBody(const std::string &path, bool repertoryListing, int rootLength)
{
	std::string body;
	if (repertoryListing)
	{
		_contentType = "text/html";
		DIR *dir;
		struct dirent *ent;
		std::cout << LOG_COLOR << "[LOG]" << " Repertory listing" << RESET << std::endl;
		if ((dir = opendir(path.c_str())) != NULL)
		{
			body += "<html><body><h1>Repertory Listing of " + path + "</h1></body></html>";
			while ((ent = readdir(dir)) != NULL)
			{
				std::cout << LOG_COLOR2 << "\t" <<ent->d_name << RESET << std::endl;
				std::string href = "images/" + std::string(ent->d_name);
				(void)rootLength;
				body += "<li><a href=\"" + href + "\">" + std::string(ent->d_name) + "</a></li>";
			}
			closedir(dir);
		}

		return (body);
	}
	else
	{
		std::cout << ERR_COLOR << "[ERR]" << "Repertory listing not allowed" << RESET << std::endl;
		_statusCode = 404; // Not Found
		return ("");
	}

}

std::string Response::_setHeaders(const HTTPRequest &request, int bodyLength)
{
	std::string headers;

	headers = _httpProtocolVersion + " " + std::to_string(_statusCode) + " " + getStatusCodeMessage(_statusCode) + "\r\n";
	headers += "Content-Type: " + _contentType + "\r\n";
	headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";


	if (request.getHeader("Connection") == "close")
		headers += "Connection: close\r\n";
	else
		headers += "Connection: keep-alive\r\n";

	headers += "Location: " + request.getPath() + "\r\n";

	return (headers);
}

bool Response::_isError()
{
	if (_statusCode < 200 || _statusCode > 301) // 301 is not an error
		return true;
	return false;
}

// /* ****** Getters ****** */
const std::string Response::getResponse() const
{
	return (_response);
}
