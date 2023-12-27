#ifndef SERVERMANAGER_CPP
# define SERVERMANAGER_CPP

#include "server.hpp"
#include "../exception/exceptions.hpp"


class ServerManager
{
	std::vector<t_server>	_serverConfigs;
	std::vector<Server>		_servers;
	std::vector<int>		_childPids;
	ServerManager();

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		~ServerManager();

		void	launchServers();
		void	stopServers();
};

#endif
