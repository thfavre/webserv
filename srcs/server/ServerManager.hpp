#ifndef SERVERMANAGER_CPP
# define SERVERMANAGER_CPP

#include "server.hpp"
#include "../exception/exceptions.hpp"
#include <sys/wait.h>

class ServerManager
{
	struct epfd {
		pollfd		pfd;
		std::string	server_name;
		bool		is_listening_socket;
		bool		keep_alive;
		std::string	response;

		epfd(std::string name, std::string response)
			: pfd(), server_name(std::move(name)), is_listening_socket(false), keep_alive(true), response(std::move(response)) {}
	};

	std::vector<t_server>		_serverConfigs;
	std::vector<Server>			_servers;
	std::vector<struct epfd>	_fds;

	ServerManager();
	void	closeSingleSocket(int index);
	void	stopServers();

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		~ServerManager();

		void		launchServers();
		epfd		makeEpfd(int fd, std::string server_name, bool is_listening_socket);
		Server		&getServerByName(const std::string &name);
		void		checkLogs(std::vector<struct epfd> fds);
};

#endif
