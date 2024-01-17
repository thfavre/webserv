#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstring>
#include <poll.h>
#include "../request/HTTPRequest.hpp"
#include "../response/Response.hpp"
#include "../Configuration/ConfigParse.hpp"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

#define MAX_CONNECTION 500
#define MAX_REQUEST_SIZE 1000000
#define WAITING 0
#define UNSET -1
#define NO_SIGNAL -1

class Server
{
	std::string				_name;
	int						_listening_socket;
	int						_sockaddr_len;
	t_server				_server_config;
	sockaddr_in				_sockaddr;
	Server();

	public:
		Server(const t_server &server_config);
		Server(const Server &src);
		~Server();

		int			acceptClient(int server_fd);
		std::string	handleRequest(int fd, bool *keep_alive);
		int			sendResponse(int fd, std::string response);
		int			getListeningSocket();
		std::string	getName();
};

#endif
