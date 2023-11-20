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
		Server(const Server &src) = delete;
		Server	&operator=(const Server &src) = delete;

		void	setup();
		void	handle_request(std::string const &request_raw);
		void	accept_connection();
		void	read_data(int i);
		void	send_response(int i);
		void	run();
		void	close(int connection);
};

#endif
