#include "server.hpp"

Server::Server()
{
	memset(&_fds, UNSET, sizeof(_fds));
}

Server::Server(const t_server &server) : _server(server)
{
	memset(&_fds, UNSET, sizeof(_fds));
}

Server::~Server()
{

}

void	Server::setup()
{
	/*
		Need to reset and prepare the _fds[]
		Need to prepare the listening socket on port (socket(), bind(), listen())
	*/

	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listening_socket == UNSET)
	{
		perror("listening socket issue");
		return ;
	}
	int on = 1;
	if (setsockopt(this->_listening_socket, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		perror("setsockopt() failed");
		close(this->_listening_socket);
		exit(-1);
	}
	std::cout << "port " << this->_server.port << std::endl;
	std::cout << "listening_socket " << this->_listening_socket << std::endl;

	memset(&this->_sockaddr, 0, sizeof(this->_sockaddr));
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(this->_server.port);
	_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(this->_listening_socket, reinterpret_cast<const sockaddr *>(&_sockaddr), sizeof(_sockaddr)) == UNSET)
	{
		perror("listening socket bind");
		close(this->_listening_socket);
		return ;
	}

	if (listen(this->_listening_socket, 10) == UNSET)
	{
		perror("listen");
		close(this->_listening_socket);
		return ;
	}

	int flags = fcntl(this->_listening_socket, F_GETFL);
	flags |= O_NONBLOCK;

	if (fcntl(this->_listening_socket, F_SETFL, flags) < 0) {
		perror("fcntl() error");
	}

	for (int i = 0; i <= MAX_CONNECTION; i++)
	{
		if (this->_fds[i].fd <= 3)
		{
			close (this->_fds[i].fd);
			this->_fds[i].fd = UNSET;
		}
		this->_fds[i].fd = UNSET;
		this->_fds[i].events = POLLIN;
		this->_fds[i].revents = 0;
	}

	this->_fds[0].fd = this->_listening_socket;
	this->_fds[0].events = POLLIN;
}

void	Server::acceptClient(const int &index)
{
	/*
		Check if there is enough place available for new connection
	*/
	int available_fd;
	sockaddr_in	client_addr;

	if ((available_fd = availableFd()) != NO_SIGNAL)
	{
		socklen_t	addr_len = sizeof(client_addr);
		int client_fd = accept(this->_fds[index].fd, (struct sockaddr *)&client_addr, &addr_len);
		if (client_fd == NO_SIGNAL)
		{
			this->_fds[available_fd].fd = UNSET;
			perror("issue accepting signal");
		}
		std::cout << "accepting client " << available_fd << std::endl;
		this->_fds[available_fd].fd = client_fd;
		this->_fds[available_fd].events = POLLIN;
		this->_fds[available_fd].revents = 0;
	}
	else
		perror ("No available fd at the moment");
}

int		Server::availableFd()
{
	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		if (this->_fds[i].fd == UNSET)
		{
			std::cout << "available fd " << this->_fds[i].fd << std::endl;
			this->_fds[i].fd = WAITING;
			return i;
		}
	}
	return -1;
}

void	Server::handleRequest(const int &index)
{
	/*
		get the request as a string for the request parser
		Check kind of request (if not done in parsing)
		Redirect it to the correct place
	*/
	char	request_buffer[MAX_REQUEST_SIZE];
	int		bytes_received = read(this->_fds[index].fd, request_buffer, MAX_REQUEST_SIZE);
	if (bytes_received == NO_SIGNAL)
	{
		perror("issue recv");
		close(this->_fds[index].fd);
		return ;
	}

	std::string raw_request(request_buffer, bytes_received);
	std::cout << "Raw_request: " << raw_request << std::endl;
	HTTPRequest	request(raw_request);
	// Response response(request, this->_fds[index].fd);
}

void	Server::sendResponse(const int &index)
{
	/*
		Not sure if done directly from Request to the Response
		Make liaison if needed with checks for errors
		To see with Thomas
	*/
	(void)index;
}

void	Server::run()
{
	/*
		while loop:
			poll()
			verify the signal
			if new signal, accept
			if client signal, handle request
			if from CGI, not sure yet
	*/

	// int	sig_index;
	while (true)
	{
		if (poll(_fds, MAX_CONNECTION, 1000) < 0)
			perror("poll() error");

		// if ((sig_index == getPollSig()) == NO_SIGNAL)
		// 	perror("issue no signal, end server");
		// std::cout << "poll connection " << this->_fds[0].fd << std::endl;

		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			// std::cout << "fd[" << i << "].fd: " << this->_fds[i].fd << std::endl;
			// std::cout << "fd[" << i << "].events: " << this->_fds[i].events << std::endl;
			// std::cout << "fd[" << i << "].revents: " << this->_fds[i].revents << std::endl;
			// std::cout << this->_fds[i].fd << " " << this->_listening_socket << "revents" << this->_fds[i].revents << std::endl;
			if (_fds[i].fd == UNSET)
				continue;
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _listening_socket)
					acceptClient(i);
				else
					handleRequest(i);
			}
			else if (this->_fds[i].revents & POLLOUT)
				sendResponse(i);
			else if (this->_fds[i].revents & (POLLERR | POLLHUP) && _fds[i].fd != _listening_socket)
			{
				perror("error on established connection");
				close(this->_fds[i].fd);
			}
		}
	}
}

void	Server::closeSingle(const int &index)
{
	//close a single fd and reset it (UNSET, revents and events too)
	if (this->_fds[index].fd >= 3)
		close(this->_fds[index].fd);
	this->_fds[index].fd = UNSET;
	this->_fds[index].events = 0;
	this->_fds[index].revents = 0;
}

void	Server::closeAll()
{
	//close every _fds[]
	for (int i = 0; i < MAX_CONNECTION; i++)
		closeSingle(i);
}

void	Server::end()
{
	closeAll();
	std::cout << "End of server " << this->_server.server_name << std::endl;
}











// Server::Server()
// {
// 	this->_listening_socket = -1;
// 	memset(this->_fds, 0, sizeof(this->_fds));
// }

// Server::Server(const t_server &server) : _server(server)
// {
// 	this->_listening_socket = -1;
// 	memset(this->_fds, 0, sizeof(this->_fds));
// }

// Server::~Server()
// {
// 	if (this->_listening_socket != -1) {
// 		close(this->_listening_socket);
// 	}

// 	for (int i = 1; i < MAX_FDS; ++i) {
// 		if (this->_fds[i].fd != -1) {
// 			close(this->_fds[i].fd);
// 		}
// 	}
// }

// // Server::Server (const Server &src)
// // {
// // 	// if (*this != src)
// // 	// 	this = src;
// // 	// return *this;
// // }

// // Server	&Server::operator=(const Server &src)
// // {

// // }

// void	Server::setup()
// {
// 	this->_sockaddr.sin_family = AF_INET;
// 	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
// 	this->_sockaddr.sin_port = htons(this->_server.port);		//TODO: check if port is <= 0 || > 65535
// 	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->_listening_socket == -1)
// 	{
// 		perror("listening_socket");
// 		/* TODO: handle the error */
// 	}

// 	// if (bind(this->_listening_socket, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
// 	// {
// 	// 	std::cout << "Failed to bind to port " << this->_server.port << ". errno: " << errno << std::endl;
// 	// 	exit(EXIT_FAILURE);
// 	// }

// 	// if (::listen(this->_listening_socket, SOMAXCONN) < 0)
// 	// {
// 	// 	std::cout << "Failed to grab connection. errno: " << errno << std::endl;
// 	// 	exit(EXIT_FAILURE);
// 	// }

// 	// this->_fds[0].fd = this->_listening_socket;
// 	// this->_fds[0].events = POLLIN;
// }

// void	Server::handle_request(std::string const &request_raw)
// {
// 	HTTPRequest	request(request_raw);

// 	if (!request.isError())	//TODO: make appropriate error throw
// 	{
// 		std::cout << BOLD << "Request Infos:" << RESET << std::endl;
// 		std::cout << YELLOW << request << RESET << std::endl;

// 		std::cout << BOLD << "Get individual:" << RESET << std::endl;
// 		std::cout << "request.getHttpProtocolVersion(): " << CYAN << request.getHttpProtocolVersion() << RESET << std::endl;
// 		std::cout << "request.getHeader(\"Host\"): " << CYAN << request.getHeader("Host") << RESET << std::endl;
// 	}

// }

// // void    Server::accept_connection()
// // {
// // 	int new_socket = accept(this->_listening_socket, NULL, NULL);
// // 	if (new_socket != -1)
// // 	{
// // 		for (int i = 1; i < MAX_FDS; i++)
// // 		{
// // 			if (this->_fds[i].fd == -1)
// // 			{
// // 				this->_fds[i].fd = new_socket;
// // 				this->_fds[i].events = POLLIN;
// // 				// Bind and listen on the new socket
// // 				if (bind(this->_fds[i].fd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
// // 				{
// // 					perror("bind");
// // 					close(this->_fds[i].fd);
// // 					this->_fds[i].fd = -1;
// // 				}
// // 				if (listen(this->_fds[i].fd, SOMAXCONN) < 0)
// // 				{
// // 					perror("listen");
// // 					close(this->_fds[i].fd);
// // 					this->_fds[i].fd = -1;
// // 				}

// // 				std::cout << "Accepted new connection on socket " << new_socket << std::endl;
// // 				break;
// // 			}
// // 		}
// // 	} else {
// // 		//TODO: throw error exception
// // 		std::cout << "Error accept: " << new_socket << std::endl;
// // 		return ;
// // 	}
// // }


// // void	Server::accept_connection()
// // {
// // 	int	new_socket = accept(this->_listening_socket, NULL, NULL);
// // 	if (new_socket != -1)
// // 	{
// // 		for (int i = 1; i < MAX_FDS; i++)
// // 		{
// // 			std::cout << "This fd: " << this->_fds[i].fd << std::endl;
// // 			if (this->_fds[i].fd == -1)
// // 			{
// // 				this->_fds[i].fd = new_socket;
// // 				this->_fds[i].events = POLLIN;
// // 				std::cout << "Accepted new connection on socket " << new_socket << std::endl;
// // 				break;
// // 			}
// // 		}
// // 	} else {
// // 		//TODO: throw error exception
// // 		std::cout << "Error accept: " << new_socket << std::endl;
// // 		return ;
// // 	}
// // }

// void	Server::close(int connection)
// {
// 	::close(connection);
// 	::close(this->_listening_socket);
// }

// void	Server::run()
// {
// 	this->_fds[0].fd = this->_listening_socket;
// 	this->_fds[0].events = POLLIN;

// 	for (int i = 1; i < MAX_FDS; i++)
// 		this->_fds[i].fd = -1;	// -1 means that the entry is available

// 	while(true)
// 	{
// 		std::cout << "File descriptors before poll:" << std::endl;
// 		for (int i = 0; i < MAX_FDS; i++)
// 		{
// 			std::cout << "FD[" << i << "]: " << this->_fds[i].fd << std::endl;
// 		}

// 		int	ret = poll(this->_fds, MAX_FDS, -1);
// 		if (ret < 0)
// 		{
// 			perror("poll");
// 			return ;
// 		}

// 		// std::cout << "File descriptors after poll:" << std::endl;
// 		// for (int i = 0; i < MAX_FDS; i++)
// 		// {
// 		// 	std::cout << "FD[" << i << "]: " << this->_fds[i].fd << std::endl;
// 		// }

// 		if (ret > 0)
// 		{
// 			if (this->_fds[0].revents & POLLIN)
// 				accept_connection();
// 			for (int i = 1; i < MAX_FDS; i++)
// 			{
// 				// std::cout << this->_fds[i].revents << std::endl;
// 				if (this->_fds[i].fd != -1)
// 				{
// 					if (this->_fds[i].revents & POLLIN)
// 						read_data(this->_fds[i]);
// 					if (this->_fds[i].revents & POLLOUT)
// 						send_response(this->_fds[i]);
// 					if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
// 					{
// 						close(this->_fds[i].fd);
// 						this->_fds[i].fd = -1;
// 					}
// 					this->_fds[i].revents = 0;
// 				}
// 			}
// 		}
// 	}
// }

// void	Server::read_data(pollfd fd)
// {
// 	char		buffer[1024];
// 	std::string	request;
// 	ssize_t		bytes_read;
// 	// ssize_t	bytes_read = recv(fd.fd, buffer, sizeof(buffer), 0);

// 	// if (bytes_read > 0) {
// 	do {
// 		bytes_read = recv(fd.fd, buffer, sizeof(buffer), 0);
// 		if (bytes_read < 0)
// 		{
// 			if (errno != EWOULDBLOCK && errno != EAGAIN)
// 				break;
// 		}
// 		else
// 		{
// 			perror("recv");
// 			close(fd.fd);
// 			fd.fd = -1;
// 		}
// 		if (request.size() + bytes_read > this->_server.client_max_body_size) {
// 			// Request size exceeded the limit, close the connection.
// 			perror("exceeded client_max_body_size");
// 			close(fd.fd);
// 			fd.fd = -1;
// 			return;
// 		}
// 		request.append(buffer, bytes_read);
// 		if (request.find("\r\n\r\n") != std::string::npos) {
// 			break;
// 		}
// 	} while (true);
// 	std::cout << "Request: " << request << std::endl;
// 	handle_request(request);
// }

// // void	Server::send_response(pollfd fd)
// // {
// // 	std::string httpResponse =
// // 		"HTTP/1.1 200 OK\r\n"
// // 		"Content-Type: text/html; charset=UTF-8\r\n"
// // 		"\r\n"
// // 		"<html>\r\n"
// // 		"<head>\r\n"
// // 		"<title>Test Page</title>\r\n"
// // 		"</head>\r\n"
// // 		"<body>\r\n"
// // 		"<h1>Hello, World!</h1>\r\n"
// // 		"<p>This is a hardcoded response from your server.</p>\r\n"
// // 		"</body>\r\n"
// // 		"</html>\r\n";
// // 	/* All of the above variables might need to be defined elsewhere */

// // 	ssize_t	bytes_sent = send(fd.fd, httpResponse.c_str(), httpResponse.size(), 0);

// // 	if (bytes_sent == -1)
// // 	{
// // 		if (errno != -1)
// // 		{
// // 			perror("send");
// // 			/* TODO: handle the error (close socket or else...) */
// // 		}
// // 	}
// // 	// } else {
// // 	// 	response_sent += bytes_sent;

// // 	// 	if (response_sent == response_len)
// // 	// 	{
// // 	// 		response_sent = 0;
// // 	// 		fd.events = POLLIN;
// // 	// 	}
// // 	// }
// // }

// // void Server::send_response(pollfd fd)
// // {
// // 	std::string httpResponse =
// // 		"HTTP/1.1 200 OK\r\n"
// // 		"Content-Type: text/html; charset=UTF-8\r\n";

// // 	std::string responseBody =
// // 		"<html>\r\n"
// // 		"<head>\r\n"
// // 		"<title>Test Page</title>\r\n"
// // 		"</head>\r\n"
// // 		"<body>\r\n"
// // 		"<h1>Hello, World!</h1>\r\n"
// // 		"<p>This is a hardcoded response from your server.</p>\r\n"
// // 		"</body>\r\n"
// // 		"</html>\r\n";

// // 	// Calculate the Content-Length based on the response body length
// // 	std::string contentLengthHeader = "Content-Length: " + std::to_string(responseBody.length()) + "\r\n";

// // 	httpResponse += contentLengthHeader;
// // 	httpResponse += "\r\n"; // End of headers

// // 	ssize_t bytes_sent = send(fd.fd, httpResponse.c_str(), httpResponse.size(), 0);

// // 	if (bytes_sent == -1)
// // 	{
// // 		perror("send");
// // 		// Handle the error (e.g., close the socket)
// // 		close(fd.fd);
// // 		fd.fd = -1;
// // 	}
// // 	else if (bytes_sent != static_cast<ssize_t>(httpResponse.size()))
// // 	{
// // 		size_t remainingBytes = httpResponse.size() - bytes_sent;
// // 		const char* remainingData = httpResponse.c_str() + bytes_sent;

// // 		while (remainingBytes > 0)
// // 		{
// // 			ssize_t bytes_sent_partial = send(fd.fd, remainingData, remainingBytes, 0);
// // 			if (bytes_sent_partial == -1)
// // 			{
// // 				perror("send");
// // 				// Handle the error (e.g., close the socket)
// // 				close(fd.fd);
// // 				fd.fd = -1;
// // 				break;
// // 			}
// // 			remainingBytes -= bytes_sent_partial;
// // 			remainingData += bytes_sent_partial;
// // 		}
// // 	}
// // 	else
// // 	{
// // 		// Successfully sent the headers, now send the response body
// // 		bytes_sent = send(fd.fd, responseBody.c_str(), responseBody.size(), 0);

// // 		if (bytes_sent == -1)
// // 		{
// // 			perror("send");
// // 			// Handle the error (e.g., close the socket)
// // 			close(fd.fd);
// // 			fd.fd = -1;
// // 		}
// // 	}
// // }










// void Server::accept_connection()
// {
//     int new_socket = accept(this->_listening_socket, NULL, NULL);

//     if (new_socket == -1)
//     {
//         perror("accept");
//         return;
//     }

//     // Check if the socket was bound and listened successfully
//     if (bind(new_socket, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) == -1)
//     {
//         close(new_socket);
//         return;
//     }

//     if (listen(new_socket, SOMAXCONN) == -1)
//     {
//         close(new_socket);
//         return;
//     }

//     for (int i = 1; i < MAX_FDS; i++)
//     {
//         if (this->_fds[i].fd == -1)
//         {
//             this->_fds[i].fd = new_socket;
//             this->_fds[i].events = POLLIN;
//             break;
//         }
//     }
// }

// void Server::send_response(pollfd fd)
// {
//     // Send the HTTP headers
//     std::string httpResponse =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/html; charset=UTF-8\r\n"
//         "Content-Length: " + std::to_string(responseBody.length()) + "\r\n"
//         "\r\n";

//     ssize_t bytes_sent = send(fd.fd, httpResponse.c_str(), httpResponse.size(), 0);

//     if (bytes_sent == -1)
//     {
//         perror("send");
//         // Handle the error (e.g., close the socket)
//         close(fd.fd);
//         fd.fd = -1;
//     }
//     else if (bytes_sent != static_cast<ssize_t>(httpResponse.size()))
//     {
//         size_t remainingBytes = httpResponse.size() - bytes_sent;
//         const char* remainingData = httpResponse.c_str() + bytes_sent;

//         while (remainingBytes > 0)
//         {
//             ssize_t bytes_sent_partial = send(fd.fd, remainingData, remainingBytes, 0);
//             if (bytes_sent_partial == -1)
//             {
//                 perror("send");
//                 // Handle the error (e.g., close the socket)
//                 close(fd.fd);
//                 fd.fd = -1;
//                 break;
//             }
//             remainingBytes -= bytes_sent_partial;
//             remainingData += bytes_sent_partial;
//         }
//     }

//     // Send the HTTP body
//     bytes_sent = send(fd.fd, responseBody.c_str(), responseBody.length(), 0);

//     if (bytes_sent == -1)
//     {
//         perror("send");
//         // Handle the error (e.g., close the socket)
//         close(fd.fd);
//         fd.fd = -1;
//     }
// }


// void	Server::end()
// {

// }

/* IDEA TO HANDLE ERRORS IN SETUP

	if (!myServer.setup(8080)) {
		std::cerr << "Failed to setup server" << std::endl;
		return -1;
	}

	So make the Server::setup() a bool function
*/
