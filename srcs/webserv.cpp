#include <iostream>
#include "request/HTTPRequest.hpp"
#include "server/server.hpp"
#include "server/ServerManager.hpp"

void httpRequestTest();

int main(int ac, char **av)
{
	(void) av;
	if (ac != 2)
	{
		std::cout << "Invalid number of arguments, please provide a configuration file" << std::endl;
		return 1;
	}
	std::vector<t_server> mockConfigs;

	// Mock configuration for site1.com
	t_server site1Config;
	site1Config.server_name = "site1.com";
	site1Config.port = 8080;
	site1Config.client_max_body_size = "200k";
	site1Config.error_pages[404] = "www/errors/404.html";
	site1Config.error_pages[405] = "www/errors/405.html";
	site1Config.routes["/admin"]["root"] = "./www/site1/";
	site1Config.routes["/admin"]["repertory_listing"] = "true";

	mockConfigs.push_back(site1Config);

	// Mock configuration for site2.com
	t_server site2Config;
	site2Config.server_name = "site2.com";
	site2Config.port = 8081;
	site2Config.client_max_body_size = "2k";
	site2Config.error_pages[404] = "./www/errors/404.html";
	site2Config.routes["/"]["root"] = "./www/site2/";
	site2Config.routes["/"]["repertory_listing"] = "false";

	mockConfigs.push_back(site2Config);

	ServerManager	serverManager(mockConfigs);
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
