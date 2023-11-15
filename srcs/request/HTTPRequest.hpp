#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <iostream>
#include <map>
#include <set>
#include <vector>



class HTTPRequest
{
public:
	// ? static variable something like parseError or parsingComplete
	// HTTPRequest();
	HTTPRequest(const std::string &requestData);
	friend std::ostream &operator<<(std::ostream &stream, const HTTPRequest &request);
	const std::string &getMethod() const;
	const std::string &getPath() const;
	const std::string &getHttpProtocolVersion() const;
	const std::string &getHeader(const std::string &headerName) const;
	const std::string &getBody() const;

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

	std::string _requestMethod;					 // GET, POST, PUT, DELETE, HEAD // TODO remove request suffix?
	std::string _requestPath;					 // /index.html
	std::string _httpProtocolVersion;			 // HTTP/1.1
	std::map<std::string, std::string> _headers; // Host: www.google.com
	std::string _body;							 // <html>...</html>

	// const std::map<std::string, std::string> _getHeaders() const;

	static const std::set<std::string> _initAcceptedMethods(); // TODO const?
	static const std::set<std::string> _initAcceptedHTTPProtocolVersions();

	// parse
	void _parseRequest(std::string requestData);
	void _parseRequestLine(const std::string &requestLine);
	void _parseHeaders(const std::string &headersLines); // TODO call it Header (same for _headers)?
	void _parseHeaderLine(const std::string &headerLine);
	void _parseMethod(const std::string &method);
	void _parsePath(const std::string &path);
	void _parseHttpProtocolVersion(const std::string &httpProtocolVersion);
	void _parseBody(const std::string &bodyLines);
};

#endif // HTTPREQUEST_HPP
