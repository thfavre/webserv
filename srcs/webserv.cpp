#include <iostream>
#include "request/HTTPRequest.hpp"
#include "server/server.hpp"
#include "server/ServerManager.hpp"
#include "Configuration/ConfigCheck.hpp"
#include "Configuration/ConfigParse.hpp"

void httpRequestTest();

int main(int ac, char **av)
{
	(void) av;
	if (ac != 2)
	{
		std::cout << "Invalid number of arguments, please provide a configuration file" << std::endl;
		return 1;
	}
	ConfigCheck		config(av[1]);
	ConfigParse		parse(config);
	ServerManager	serverManager(parse.getServersParsed());
	try {
		serverManager.launchServers();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

void httpRequestTest()
{
	std::string requestData = "GET /index.html HTTP/1.1\r\n"
							  "Host: www.google.com\r\n"
							  "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\r\n"
							  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
							  "Accept-Language: en-US,en;q=0.5\r\n"
							  "Accept-Encoding: gzip, deflate, br\r\n"
							  "Connection: keep-alive\r\n"
							  "Upgrade-Insecure-Requests: 1\r\n"
							  "Cache-Control: max-age=0\r\n"
							  "\r\n"
							  "<html>...</html>";
	HTTPRequest request(requestData);

	if (!request.isError())
	{

		#define YELLOW "\033[33m"
		#define CYAN "\033[36m"
		#define RED "\033[31m"
		#define BOLD "\033[1m"
		#define RESET "\033[0m"
		std::cout << BOLD << "Request Infos:" << RESET << std::endl;
		std::cout << YELLOW << request << RESET << std::endl;

		std::cout << BOLD << "Get individual:" << RESET << std::endl;
		std::cout << "request.getHttpProtocolVersion(): " << CYAN << request.getHttpProtocolVersion() << RESET << std::endl;
		std::cout << "request.getHeader(\"Host\"): " << CYAN << request.getHeader("Host") << RESET << std::endl;
	}
}
