#ifndef SERVER_HPP
# define SERVER_HPP

// #include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
// #include <map>
// #include <vector>
#include <poll.h>
#include "../request/HTTPRequest.hpp"
#include "../Configuration/ConfigParse.hpp"

#define MAX_FDS 200

#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

class Server
{
	int						_listening_socket;
	std::vector<t_server>	_configs;
	sockaddr_in				_sockaddr;
	pollfd					_fds[MAX_FDS];


	public:
		Server();
		~Server();
		// Server(const Server &src);
		// Server	&operator=(const Server &src);

		void	setup(const t_server &config);
		void	handle_request(std::string const &request_raw);
		void	accept_connection();
		void	read_data(pollfd fd);
		void	send_response(pollfd fd);
		void	run();
		void	close(int connection);
		void	end();

		void	setConfigs(std::vector<t_server> &configs);
};

#endif
