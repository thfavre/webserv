#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <string>
# include <map>
# include "../request/HTTPRequest.hpp"

class Response
{
	public:
		Response(const HTTPRequest &request, int socketFd);
		const std::string getResponse() const;

	private:
		std::string _httpProtocolVersion;
		int _statusCode;
		std::string _statusMessage;
		// std::map<std::string, std::string> _headers;
		// std::string _body;
		std::string _response;

		void _formatResponse(const HTTPRequest &request);
		void _sendResponse(int socketFd);
};


#endif
