#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <string>
# include <map>
# include "HTTPRequest.hpp"

class Response
{
	public:
		Response(const HTTPRequest &request);

	private:
		std::string _httpProtocolVersion;
		int _statusCode;
		std::string _statusMessage;
		// std::map<std::string, std::string> _headers;
		// std::string _body;
		std::string _response;

		void formatResponse();
};


#endif
