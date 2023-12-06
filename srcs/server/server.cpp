#include "server.hpp"

Server::Server()
{
	this->_listening_socket = -1;
	memset(this->_fds, 0, sizeof(this->_fds));
}

Server::~Server()
{
	if (this->_listening_socket != -1) {
		close(this->_listening_socket);
	}

	for (int i = 1; i < MAX_FDS; ++i) {
		if (this->_fds[i].fd != -1) {
			close(this->_fds[i].fd);
		}
	}
}

// Server::Server (const Server &src)
// {
// 	// if (*this != src)
// 	// 	this = src;
// 	// return *this;
// }

// Server	&Server::operator=(const Server &src)
// {

// }

void	Server::setup(const t_server &config)
{
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(config.port);		//TODO: check if port is <= 0 || > 65535
	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listening_socket == -1)
	{
		perror("listening_socket");
		/* TODO: handle the error */
	}

	if (bind(this->_listening_socket, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << config.port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	if (::listen(this->_listening_socket, SOMAXCONN) < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	this->_fds[0].fd = this->_listening_socket;
	this->_fds[0].events = POLLIN;
}

void	Server::handle_request(std::string const &request_raw)
{
	HTTPRequest	request(request_raw);

	if (!request.isError())
	{
		std::cout << BOLD << "Request Infos:" << RESET << std::endl;
		std::cout << YELLOW << request << RESET << std::endl;

		std::cout << BOLD << "Get individual:" << RESET << std::endl;
		std::cout << "request.getHttpProtocolVersion(): " << CYAN << request.getHttpProtocolVersion() << RESET << std::endl;
		std::cout << "request.getHeader(\"Host\"): " << CYAN << request.getHeader("Host") << RESET << std::endl;
	}
}

void	Server::accept_connection()
{
	int	new_socket = accept(this->_listening_socket, NULL, NULL);
	if (new_socket != -1)
	{
		for (int i = 1; i < MAX_FDS; i++)
		{
			if (this->_fds[i].fd == -1)
			{
				this->_fds[i].fd = new_socket;
				this->_fds[i].events = POLLIN;
				break;
			}
		}
	}
}

void	Server::close(int connection)
{
	::close(connection);
	::close(this->_listening_socket);
}

void	Server::run()
{
	this->_fds[0].fd = this->_listening_socket;
	this->_fds[0].events = POLLIN;

	for (int i = 1; i < MAX_FDS; i++)
		this->_fds[i].fd = -1;	// -1 means that the entry is available

	while(true)
	{
		int	ret = poll(this->_fds, MAX_FDS, -1);
		if (ret <= 0)
		{
			perror("poll");
			return ;
		}

		if (ret > 0)
		{
			if (this->_fds[0].revents & POLLIN)
				accept_connection();
			for (int i = 1; i < MAX_FDS; i++)
			{
				if (this->_fds[i].fd != -1)
				{
					if (this->_fds[i].revents & POLLIN)
						read_data(this->_fds[i]);
					if (this->_fds[i].revents & POLLOUT)
						send_response(this->_fds[i]);
					if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
					{
						close(this->_fds[i].fd);
						this->_fds[i].fd = -1;
					}
				}
			}
		}
	}
}

void	Server::read_data(pollfd fd)
{
	char	buffer[1024];
	ssize_t	bytes_read = recv(fd.fd, buffer, sizeof(buffer), 0);

	if (bytes_read > 0) {
		std::string	request;
		do {
			request.append(buffer, bytes_read);
			if (request.find("\r\n\r\n") != std::string::npos) {
				break;
			}
		} while ((bytes_read = recv(fd.fd, buffer, sizeof(buffer), 0)) > 0);
		handle_request(request);
	} else if (bytes_read == 0) {
		close(fd.fd);
		fd.fd = -1;
	} else {
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			perror("recv");
			close(fd.fd);
			fd.fd = -1;
		}
	}
}

// void	Server::send_response(pollfd fd)
// {
// 	std::string httpResponse =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: text/html; charset=UTF-8\r\n"
// 		"\r\n"
// 		"<html>\r\n"
// 		"<head>\r\n"
// 		"<title>Test Page</title>\r\n"
// 		"</head>\r\n"
// 		"<body>\r\n"
// 		"<h1>Hello, World!</h1>\r\n"
// 		"<p>This is a hardcoded response from your server.</p>\r\n"
// 		"</body>\r\n"
// 		"</html>\r\n";
// 	/* All of the above variables might need to be defined elsewhere */

// 	ssize_t	bytes_sent = send(fd.fd, httpResponse.c_str(), httpResponse.size(), 0);

// 	if (bytes_sent == -1)
// 	{
// 		if (errno != -1)
// 		{
// 			perror("send");
// 			/* TODO: handle the error (close socket or else...) */
// 		}
// 	}
// 	// } else {
// 	// 	response_sent += bytes_sent;

// 	// 	if (response_sent == response_len)
// 	// 	{
// 	// 		response_sent = 0;
// 	// 		fd.events = POLLIN;
// 	// 	}
// 	// }
// }

void Server::send_response(pollfd fd)
{
	std::string httpResponse =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n";

	std::string responseBody =
		"<html>\r\n"
		"<head>\r\n"
		"<title>Test Page</title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<h1>Hello, World!</h1>\r\n"
		"<p>This is a hardcoded response from your server.</p>\r\n"
		"</body>\r\n"
		"</html>\r\n";

	// Calculate the Content-Length based on the response body length
	std::string contentLengthHeader = "Content-Length: " + std::to_string(responseBody.length()) + "\r\n";

	httpResponse += contentLengthHeader;
	httpResponse += "\r\n"; // End of headers

	ssize_t bytes_sent = send(fd.fd, httpResponse.c_str(), httpResponse.size(), 0);

	if (bytes_sent == -1)
	{
		perror("send");
		// Handle the error (e.g., close the socket)
		close(fd.fd);
		fd.fd = -1;
	}
	else if (bytes_sent != static_cast<ssize_t>(httpResponse.size()))
	{
		// Handle partial send if needed
		// You may need to resend the remaining data
		// You can implement logic to handle this case
	}
	else
	{
		// Successfully sent the headers, now send the response body
		bytes_sent = send(fd.fd, responseBody.c_str(), responseBody.size(), 0);

		if (bytes_sent == -1)
		{
			perror("send");
			// Handle the error (e.g., close the socket)
			close(fd.fd);
			fd.fd = -1;
		}
	}
}


void	Server::end()
{

}

void	Server::setConfigs(std::vector<t_server> &configs)
{
	_configs = configs;
}

/* IDEA TO HANDLE ERRORS IN SETUP

	if (!myServer.setup(8080)) {
		std::cerr << "Failed to setup server" << std::endl;
		return -1;
	}

	So make the Server::setup() a bool function
*/
