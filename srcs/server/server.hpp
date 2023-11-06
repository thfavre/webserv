#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{
	int				_port;
	int				_socket;
	sockaddr_in		_sockaddr;
	Server();

	public:
		Server(int port);
		~Server();

		void	setup();
		void	listen_connection();
		void	handle_request(int client_socket);
		void	accept_connection();
		void	close(int connection);
};

#endif
