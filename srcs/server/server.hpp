#ifndef SERVER_HPP
# define SERVER_HPP

// #include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
// #include <map>
// #include <vector>
#include <poll.h>
#include <fcntl.h>
#include "../request/HTTPRequest.hpp"
#include "../response/Response.hpp"
#include "../Configuration/ConfigParse.hpp"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

#define MAX_CONNECTION 50
#define MAX_REQUEST_SIZE 1000
#define WAITING 0
#define UNSET -1
#define NO_SIGNAL -1

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
		void	acceptClient(const int &index);
		void	handleRequest(const int &index);
		void	sendResponse(const int &index);
		void	run();
		void	closeSingle(const int &index);
		void	closeAll();
		void	end();

		// int		getPollSig();
		int		availableFd();
};

#endif
