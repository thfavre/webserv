#ifndef SERVERMANAGER_CPP
# define SERVERMANAGER_CPP

#include "server.hpp"

class ServerManager
{
	std::vector<t_server>	_serverConfigs;
	std::vector<Server>		_servers;
	ServerManager();

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		~ServerManager();

		void	launchServers();
		void	stopServers();
};

#endif
