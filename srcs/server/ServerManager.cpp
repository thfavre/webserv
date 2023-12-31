#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{

}

ServerManager::~ServerManager()
{

}

// void	ServerManager::launchServers()
// {
// 	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
// 	{
// 		const t_server& config = *it;

// 		Server server(config);
// 		server.setup();
// 		server.run();
// 		_servers.push_back(server);
// 	}
// }

void	ServerManager::launchServers()
{
	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	{
		const t_server& config = *it;

		Server server(config);
		server.setup();

		int pid = fork();
		if (pid == 0) {
			// Child process
			server.run();
			return;
		} else if (pid < 0) {
			perror("issue with the forking");
			//throw MyException(ErrorType::FORK, "issue when forking the server at creation");
			return;
		} else {
			// Parent process
			_childPids.push_back(pid);
			_servers.push_back(std::move(server));
		}
	}

	for (std::vector<Server>::iterator i = _servers.begin(); i != _servers.end();)
	{
		if (waitpid((*i).getPid(), NULL, 0) == -1)
		{
			perror("waitpid() failed");
			_servers.erase(i);
		}
		else
		{
			i++;
		}
	}
}

void	ServerManager::stopServers()
{
	for (size_t i = 0; i < this->_servers.size(); ++i) {
		this->_servers[i].end();
	}
}
