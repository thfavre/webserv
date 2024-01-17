#include "server.hpp"

Server::Server()
{

}

Server::Server(const t_server &server_config) : _server_config(server_config)
{
	this->_name = this->_server_config.server_name;

	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listening_socket == UNSET)
	{
		throw std::runtime_error("[EXCEPTION] listening socket issue");
	}

	int on = 1;
	if (setsockopt(this->_listening_socket, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		close(this->_listening_socket);
		throw std::runtime_error("[EXCEPTION] setsockopt() failed");
	}

	sockaddr_in	tmp_sockaddr;
	tmp_sockaddr.sin_family = AF_INET;
	tmp_sockaddr.sin_port = htons(this->_server_config.port);
	tmp_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_sockaddr = tmp_sockaddr;
	this->_sockaddr_len = sizeof(tmp_sockaddr);

	if (bind(this->_listening_socket, (struct sockaddr *) &this->_sockaddr, this->_sockaddr_len) < 0)
	{
		close(this->_listening_socket);
		throw std::runtime_error("[EXCEPTION] Binding listening socket issue");
	}

	if (listen(this->_listening_socket, 100) == UNSET)
	{
		close(this->_listening_socket);
		throw std::runtime_error("[EXCEPTION] Listen to listening socket issue");
	}

	if (fcntl(this->_listening_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		throw std::runtime_error("[EXCEPTION] Setting socket non-blocking issue");
	}

	std::cout << "Server " << this->_name << " available on port " << this->_server_config.port << std::endl;
}

Server::Server(const Server &src)
{
	this->_name = src._name;
	this->_server_config = src._server_config;
}

Server::~Server()
{

}

int			Server::acceptClient(int server_fd)
{
	sockaddr_in	client_addr;
	socklen_t	client_len = sizeof(client_addr);

	int	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0)
		throw std::runtime_error("[EXCEPTION] Issue accepting new connection");

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("[EXCEPTION] Issue setting client_fd to unblocking");
	std::cout << CYAN << "[LOG] Accepting new connection on fd " << client_fd << RESET << std::endl;
	return client_fd;
}

std::string Server::handleRequest(int fd, bool* keep_alive)
{
	char request_buffer[MAX_REQUEST_SIZE];
	ssize_t bytes_received = recv(fd, request_buffer, MAX_REQUEST_SIZE, 0);

	if (bytes_received < 0) {
		std::cerr << RED << "[ERROR] Error receiving data on fd " << fd << RESET << std::endl;
		return std::string();
	}

	if (bytes_received == 0) {
		std::cout << CYAN << "[LOG] Connection closed by the client on fd " << fd << RESET << std::endl;
		return std::string();
	}

	std::string raw_request(request_buffer, bytes_received);

	try {
		HTTPRequest request(raw_request, _server_config);
		std::cout <<"bytes recevied: " << bytes_received << std::endl;
		if (bytes_received >= (MAX_REQUEST_SIZE - 1))
		{
			std::cout << "test" << std::endl;
			request.setStatusCode(413);
		}
		Response response(request, fd, _server_config);

		if (strcasecmp(request.getHeader("Connection").c_str(), "close") == 0) {
			*keep_alive = false;
		}

		return response.getResponse();
	} catch (const std::exception& e) {
		std::cerr << RED << "[ERROR] Exception during request handling on fd " << fd << ": " << e.what() << RESET << std::endl;
		return std::string();
	}
}

int			Server::sendResponse(int fd, std::string response)
{
	ssize_t bytes_sent = send(fd, response.c_str(), response.length(), MSG_DONTWAIT);

	if (bytes_sent > 0)
	{
		if (static_cast<size_t>(bytes_sent) != response.length())
			std::cerr << RED << "[ERROR] Response not sent entirely: " << bytes_sent << "/" << response.length() << RESET << std::endl;
		else
			std::cout << CYAN << "[LOG] Response sent on fd " << fd << RESET << std::endl;
		return 1;
	}
	else if (bytes_sent == 0)
	{
		std::cout << CYAN << "[LOG] Client closed the connection on fd " << fd << RESET << std::endl;
		return 0;
	}
	else if (bytes_sent == -1)
	{
		std::cout << RED << "[ERROR] Issue sending the response on fd " << fd << RESET << std::endl;
		return 0;
	}
	return 0;
}

std::string	Server::getName()
{
	return this->_name;
}

int			Server::getListeningSocket()
{
	return this->_listening_socket;
}
