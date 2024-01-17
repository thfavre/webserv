#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include "CGIHandler.hpp"
#include "../Configuration/ConfigParse.hpp"

#define MAX_PATH_LENGTH 4096
#define LINE_END "\r\n"
#define UPLOAD_FOLDER "./webpage/images/"

class HTTPRequest
{
public:
	HTTPRequest(const std::string &requestData, const t_server &server);
	friend std::ostream &operator<<(std::ostream &stream, const HTTPRequest &request);
	const std::string &getMethod() const;
	const std::string &getPath() const;
	const std::string &getHttpProtocolVersion() const;
	const std::string &getHeader(const std::string &headerName) const;
	const std::string &getBody() const;
	const std::string &getRoot() const;
	bool getReperoryListing() const;
	const int &getStatusCode() const;
	bool isCGI() const;
	const std::string getCGIPath() const;
	const std::list<std::string> &getUrlParameters() const;
	void	setStatusCode(int code);

	class InvalidRequestException : public std::exception
	{
	public:
		InvalidRequestException(const std::string &message) : _message(message) {}
		virtual const char *what(void) const throw()
		{
			return (_message.c_str());
		}
		virtual ~InvalidRequestException() throw() {}
		InvalidRequestException(void) : _message("Invalid request") {}

	private:
		std::string _message;
	};

private:
	static const std::set<std::string> _acceptedMethods;
	static const std::set<std::string> _acceptedHTTPProtocolVersions;

	int _statusCode;
	std::string _CGIPath;
	t_server _server;
	std::map<std::string, std::string> _configRootOptions;
	std::string _configRoute;
	std::string _requestMethod;					 // GET, POST, PUT, DELETE, HEAD
	std::string _requestPath;					 // /index.html
	std::string _httpProtocolVersion;			 // HTTP/1.1
	std::map<std::string, std::string> _headers; // Host: www.google.com
	std::string _body;							 // <html>...</html>
	std::string _post_file_name;
	std::string _post_file_content;
	std::list<std::string> _urlParameters;
	bool _isCGI;

	static const std::set<std::string> _initAcceptedMethods();
	static const std::set<std::string> _initAcceptedHTTPProtocolVersions();

	// parse
	void _parseRequest(std::string requestData);
	void _parseRequestLine(const std::string &requestLine);
	void _parseHeaders(const std::string &headersLines);
	void _parseHeaderLine(const std::string &headerLine);
	void _parseMethod(const std::string &method);
	void _parsePath(std::string path);
	void _getConfigRootOptions(const std::string &requestPath);
	const std::string _getRedirectedPath(const std::string &path);
	bool _areAllPathCharactersValid(const std::string &path);
	bool _isSafePath(const std::string &path);
	bool _isPathLengthValid(const std::string &path, size_t maxLength);
	void _parseHttpProtocolVersion(const std::string &httpProtocolVersion);
	void _parseBody(const std::string &bodyLines);
	void _parseMultiPartBody(const std::string &bodyLines);
	std::string _getMultiPartBoundary();
	void _parseUrlencodedBody(const std::string &bodyLine);

	// execute
	void _executeMethod();
	void _exectuteCGI();
};

#endif // HTTPREQUEST_HPP
