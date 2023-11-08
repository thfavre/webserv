#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <iostream>
#include <map>
#include <set>

class HTTPRequest
{
public:
	// HTTPRequest();
	HTTPRequest(const std::string &requestData);
	friend std::ostream& operator<< (std::ostream& stream, const HTTPRequest& request);
	const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getHttpProtocolVersion() const;
    const std::string& getHeader(const std::string& headerName) const;
    const std::string& getBody() const;

private:
	std::set<std::string> _acceptedMethods;
	std::string _requestMethod;					 // GET, POST, PUT, DELETE, HEAD // TODO remove request suffix?
	std::string _requestPath;					 // /index.html
	std::string _httpProtocolVersion;			 // HTTP/1.1
	std::map<std::string, std::string> _headers; // Host: www.google.com
	std::string _body;					 // <html>...</html>

	const std::map<std::string, std::string> _getHeaders() const;

	// parse
	void _parseRequest(std::istringstream& requestStream);
	void _parseRequestLine(std::istringstream& requestStream);
	void _parseHeaders(std::istringstream& requestStream); // TODO call it Header (same for _headers)?
	void _parseHeaderLine(const std::string& headerLine);
	void _parseBody(std::istringstream& requestStream);

	// validation // TODO rename comment name
	bool _isMethodValid(const std::string& method) const;
};


#endif // HTTPREQUEST_HPP
