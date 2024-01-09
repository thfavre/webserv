#include "server.hpp"

Server::Server()
{
	memset(&_fds, UNSET, sizeof(_fds));
}

Server::Server(const t_server &server_config) : _server_config(server_config)
{
	this->_name = this->_server_config.server_name;

	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listening_socket == UNSET)
	{
		throw std::runtime_error("listening socket issue");
	}

	int on = 1;
	if (setsockopt(this->_listening_socket, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		close(this->_listening_socket);
		throw std::runtime_error("setsockopt() failed");
	}

	memset(&this->_sockaddr, 0, sizeof(this->_sockaddr));
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_port = htons(this->_server_config.port);
	this->_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_sockaddr_len = sizeof(this->_sockaddr);

	if (bind(this->_listening_socket, (struct sockaddr *) &this->_sockaddr, this->_sockaddr_len) < 0)
	{
		close(this->_listening_socket);
		throw std::runtime_error("Binding listening socket issue");
	}

	if (listen(this->_listening_socket, 10) == UNSET)
	{
		close(this->_listening_socket);
		throw std::runtime_error("Listening to socket issue");
	}

	if (fcntl(this->_listening_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		throw std::runtime_error("Setting socket non-blocking issue");
	}

	std::cout << "Server " << this->_name << " available on port " << this->_server_config.port << std::endl;
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
		throw std::runtime_error("Issue acceptiing new connection");

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Issue setting client_fd to unblocking");

	return client_fd;
}

std::string	Server::handleRequest(int fd, bool *keep_alive)
{
	char	request_buffer[MAX_REQUEST_SIZE];
	int		bytes_received = read(fd, request_buffer, MAX_REQUEST_SIZE);
	if (bytes_received == 0)
	{
		/*
			Need to close the socket, it means that there is nothing sent from the client
		*/
	}
	if (bytes_received == NO_SIGNAL)
	{
		std::cerr << "Error reading from fd " << fd << ": " << strerror(errno) << std::endl;
		std::cout << "bytes received in case of error " << bytes_received << std::endl;
		std::cout << "and here is the fd related " << fd << std::endl;
		return (std::string());
	}

	std::string raw_request(request_buffer, bytes_received);
	std::cout << "Raw_request: " << raw_request << std::endl;
	HTTPRequest	request(raw_request, _server_config);
	Response response(request, fd, _server_config);
	if (request.getHeader("close") == "close")
		*keep_alive = false;
	return (response.getResponse());
}

void		Server::sendResponse(int fd, std::string response)
{
	if (send(fd, response.c_str(), response.length(), MSG_DONTWAIT) != -1)
	{
		std::cout << "Response sent" << std::endl;
	}
	else
	{
		throw std::runtime_error("Issue sending the response");
		/*
			TODO: Need to check if I close the socket in case of issue sending the response, or if we just reset the socket back to POLLIN
		*/
	}
}

std::string	Server::getName()
{
	return this->_name;
}

int			Server::getListeningSocket()
{
	return this->_listening_socket;
}
