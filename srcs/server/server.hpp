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

#define MAX_CONNECTION 200

#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

#define UNSET -1

class Server
{
	int						_listening_socket;
	t_server				_server;
	sockaddr_in				_sockaddr;
	pollfd					_fds[MAX_CONNECTION];


	public:
		Server();
		Server(const t_server &server);
		~Server();
		// Server(const Server &src);
		// Server	&operator=(const Server &src);

		void	setup();
		void	handleRequest(std::string const &request_raw);
		void	acceptClient();
		void	readData(pollfd fd);
		void	sendResponse(pollfd fd);
		void	run();
		void	close(const int &connection);
		void	closeSingle(const int &index);
		void	end();

		int		getPollSig();
};

#endif
