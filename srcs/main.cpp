#include <iostream>
#include "request/HTTPRequest.hpp"
#include "response/Response.hpp"

void httpRequestTest();
void webservTest();

int main()
{
	// httpRequestTest();
	webservTest();
}
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
void webservTest()
{
	int PORT = 8080;
	int domain = AF_INET;
	int type = SOCK_STREAM;
	int protocol = 0;
	int server_fd = socket(domain, type, protocol);
	printf("socket : %d\n", server_fd);

	struct sockaddr_in address;
	int addrlen = sizeof(address);

	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		return ;
	}

	if (listen(server_fd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	int new_socket;
	while (1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		// get the request string
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		char request_buffer[30000] = {0};
		read(new_socket, request_buffer, 30000);

		// parse the request
		HTTPRequest request(request_buffer);
		if (request.isError())
		{
			std::cout << "Error parsing request" << std::endl;
			continue;
		}
		const char *page_name = request.getPath().c_str();

		if (strstr(page_name, ".html") != NULL)
		{
			Response response(request, new_socket);

			// std::string responseString = response.getResponse();
			// const char *respond = responseString.c_str();

			// write(new_socket, respond, strlen(respond));
		}
		printf("done\n");
		// printf("------------------Hello message sent-------------------\n");

		close(new_socket);
	}
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
	else {
		std::cout << RED << "Error parsing request" << RESET << std::endl;
	}
}
