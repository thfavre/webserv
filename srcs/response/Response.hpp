#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <string>
# include <map>
# include "../request/HTTPRequest.hpp"

class Response
{
	public:
		Response(const HTTPRequest &request, int socketFd);
		// const std::string getResponse() const;

	private:
		std::string _httpProtocolVersion;
		int _statusCode;
		// std::string _statusMessage;
		// std::map<std::string, std::string> _headers;
		// std::string _body;
		// std::string _response;

		std::string _formatResponse(const HTTPRequest &request);
		std::string _setBody(const HTTPRequest &request);
		std::string _setHeaders(const HTTPRequest &request, int bodyLength);
		std::string _getContentType(const std::string &path);
		void _sendResponse(int socketFd, const std::string &response);
		bool _isError();
};


#endif
