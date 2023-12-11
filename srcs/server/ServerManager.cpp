#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{

}

ServerManager::~ServerManager()
{

}

void	ServerManager::launchServers()
{
	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	{
		const t_server& config = *it;

		Server server;
		server.setup(config);
		server.run();
		_servers.push_back(server);
	}
}

void	ServerManager::stopServers()
{
	for (size_t i = 0; i < this->_servers.size(); ++i) {
		this->_servers[i].end();
	}
}
