#include "Server.hpp"

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

void	Server::setup()
{
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = PORT;
	this->_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listening_socket == -1)
	{
		perror("listening_socket");
		/* TODO: handle the error */
	}

	if (bind(this->_listening_socket, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << PORT << ". errno: " << errno << std::endl;
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

void	Server::handle_request(int client_request)
{
	/*
	Might be something to do in the parser and then execute from here
	*/
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
	close(connection);
	close(this->_listening_socket);
}

void	Server::run()
{
	this->_fds[0].fd = this->_listening_socket; //TODO: define listening_socket
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
						read_data(i);
					if (this->_fds[i].revents & POLLOUT)
					{
						send_data(i);
					}
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

void	Server::read_data(int i)
{
	char	buffer[1024];
	ssize_t bytes_read	= recv(this->_fds[i].fd, buffer, sizeof(buffer), 0);

	if (bytes_read > 0)
	{
		/* TODO send the request data to the Request parser*/
	} else if (bytes_read == 0) {
		close(this->_fds[i].fd);
		this->_fds[i].fd = -1;
	} else {
		if (errno != EWOULDBLOCK)
		{
			perror("recv");
			close(this->_fds[i].fd);
			this->_fds[i].fd = -1;
		}
	}
}

void	Server::send_data(int i)
{
	const char	*response = "Probably need to make a call to some function to get the answer";
	size_t		response_len = strlen(response);
	size_t		response_sent = 0;
	/* All of the above variables might need to be defined elsewhere */

	ssize_t	bytes_sent = send(this->_fds[i].fd, response + response_sent, response_len - response_sent, 0);

	if (bytes_sent == -1)
	{
		if (errno != -1)
		{
			perror("send");
			/* TODO: handle the error (close socket or else...) */
		}
	} else {
		response_sent += bytes_sent;

		if (response_sent == response_len)
		{
			response_sent = 0;
			this->_fds[i].events = POLLIN;
		}
	}
}

/* IDEA TO HANDLE ERRORS IN SETUP

	if (!myServer.setup(8080)) {
		std::cerr << "Failed to setup server" << std::endl;
		return -1;
	}

	So make the Server::setup() a bool function
*/
