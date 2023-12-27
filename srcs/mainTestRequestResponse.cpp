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
		return;
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
		// if (request.isError())
		// {
		// 	std::cout << "Error parsing request" << std::endl;
		// 	continue;
		// }
		// const char *page_name = request.getPath().c_str();

		// if (strstr(page_name, ".html") != NULL)
		// {
		Response response(request, new_socket);

		// std::string responseString = response.getResponse();
		// const char *respond = responseString.c_str();

		// write(new_socket, respond, strlen(respond));
		// }
		printf("done\n");
		// printf("------------------Hello message sent-------------------\n");

		close(new_socket);
	}
}

void httpRequestTest()
{
	std::string getRequestData = "GET /index.html HTTP/1.1\r\n"
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
	std::string postRequestData = "POST /index.html HTTP/1.1\r\n"
								  "Host: www.google.com\r\n"
								  "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\r\n"
								  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
								  "Accept-Language: en-US,en;q=0.5\r\n"
								  "Accept-Encoding: gzip, deflate, br\r\n"
								  "Connection: keep-alive\r\n"
								  "Upgrade-Insecure-Requests: 1\r\n"
								  "Cache-Control: max-age=0\r\n"
								  "Content-Length: 11\r\n"
								  "\r\n"
								  "Hello World POST working!";
	std::string deleteRequestData = "DELETE /index.html HTTP/1.1\r\n"
									"Host: www.google.com\r\n"
									"User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\r\n"
									"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
									"Accept-Language: en-US,en;q=0.5\r\n"
									"Accept-Encoding: gzip, deflate, br\r\n"
									"Connection: keep-alive\r\n"
									"Upgrade-Insecure-Requests: 1\r\n"
									"Cache-Control: max-age=0\r\n"
									"\r\n";
	std::string cgiRequestData = "GET /testCGI.py HTTP/1.1\r\n"
								 "Host: www.google.com\r\n"
								 "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\r\n"
								 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
								 "Accept-Language: en-US,en;q=0.5\r\n"
								 "Accept-Encoding: gzip, deflate, br\r\n"
								 "Connection: keep-alive\r\n"
								 "Upgrade-Insecure-Requests: 1\r\n"
								 "Cache-Control: max-age=0\r\n"
								 "\r\n";

	HTTPRequest request(cgiRequestData);

#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"
	std::cout << BOLD << "Request status code:" << request.getStatusCode() << RESET << std::endl;
	if (request.getStatusCode() >= 200 && request.getStatusCode() < 300)
	{
		std::cout << YELLOW << "Request Infos:" << RESET << std::endl;
		std::cout << request << std::endl;

		std::cout << YELLOW << "Get individual:" << RESET << std::endl;
		std::cout << "request.getHttpProtocolVersion(): " << request.getHttpProtocolVersion() << std::endl;
		std::cout << "request.getHeader(\"Host\"): " << request.getHeader("Host") << std::endl;
		std::cout << "request.isCGI(): " << request.isCGI() << std::endl;

		// response
		std::cout << std::endl << YELLOW << "Response:" << RESET << std::endl;
		Response response(request, 1);
	}
	else
	{
		std::cout << RED << "Error, status code : " << request.getStatusCode() << RESET << std::endl;
	}
	// {

	// 	#define YELLOW "\033[33m"
	// 	#define CYAN "\033[36m"
	// 	#define RED "\033[31m"
	// 	#define BOLD "\033[1m"
	// 	#define RESET "\033[0m"
	// 	std::cout << BOLD << "Request Infos:" << RESET << std::endl;
	// 	std::cout << YELLOW << request << RESET << std::endl;

	// 	std::cout << BOLD << "Get individual:" << RESET << std::endl;
	// 	std::cout << "request.getHttpProtocolVersion(): " << CYAN << request.getHttpProtocolVersion() << RESET << std::endl;
	// 	std::cout << "request.getHeader(\"Host\"): " << CYAN << request.getHeader("Host") << RESET << std::endl;
	// }
	// else {
	// 	std::cout << RED << "Error parsing request" << RESET << std::endl;
	// }
}
