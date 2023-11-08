#include <iostream>
#include "request/HTTPRequest.hpp"

void httpRequestTest();


int main()
{
	// httpRequestTest();
}


void httpRequestTest()
{
	// Currently don't work with \r\n separator...
	std::string requestData = "GET /index.html HTTP/1.1\n"
						  "Host: www.google.com\n"
						  "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\n"
						  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n"
						  "Accept-Language: en-US,en;q=0.5\n"
						  "Accept-Encoding: gzip, deflate, br\n"
						  "Connection: keep-alive\n"
						  "Upgrade-Insecure-Requests: 1\n"
						  "Cache-Control: max-age=0\n"
						  "\n"
						  "<html>...</html>";

	HTTPRequest request(requestData);

	std::cout << request << std::endl;
}
