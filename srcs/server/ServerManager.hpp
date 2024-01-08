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
	};

	std::vector<t_server>		_serverConfigs;
	std::vector<Server>			_servers;
	std::vector<struct epfd>	_fds;

	ServerManager();

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		~ServerManager();

		void	launchServers();
		void	stopServers();
		Server	&getServerByName(std::string &name);
};

#endif
