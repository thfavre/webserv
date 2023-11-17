#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <poll.h>

#define MAX_FDS 200
#define PORT 8080

class Server
{
	int				_listening_socket;
	sockaddr_in		_sockaddr;
	pollfd			_fds[MAX_FDS];


	public:
		Server();
		~Server();

		void	setup();
		void	handle_request(int client_socket);
		void	accept_connection();
		void	read_data(int i);
		void	send_data(int i);
		void	run();
		void	close(int connection);
};

#endif
