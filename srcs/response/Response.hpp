#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <string>
# include <map>
# include "../request/HTTPRequest.hpp"

class Response
{
	public:
		Response(const HTTPRequest &request, int socketFd, const t_server &server);
		const std::string getResponse() const;

	private:
		std::string _httpProtocolVersion;
		int _statusCode;
		t_server _server;
		std::string _contentType;
		std::string _getContentType(const std::string &path);
		std::string _response;


		std::string _formatResponse(const HTTPRequest &request);
		std::string _formatGenericErrorPageHTML();
		std::string _setBody(const HTTPRequest &request);
		std::string _setCGIBody(const std::string &path, const std::string &cgiPath, const std::list<std::string> &args);
		std::string _setFileBody(const std::string &path);
		std::string _setDirectoryBody(const std::string &path, bool reperoryListing, int rootLength);
		std::string _setHeaders(const HTTPRequest &request, int bodyLength);
		bool _isError();
};


#endif
