#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{
	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	{
		const t_server& config = *it;

		Server server(config);
		server.setup();
		server.run();
		_servers.push_back(server);
	}
}

ServerManager::~ServerManager()
{

}


void	ServerManager::launchServers()
{
	/*
		while loop:
			poll()
			verify the signal
			if new signal, accept
			if client signal, handle request
			if from CGI, not sure yet
	*/

	// int	sig_index;
	while (true)
	{
		/*
			Need to redo the process of chosing which pollfd struct to use in the poll()
		*/

		if (poll(this->_fds.pfd.data(), this->_fds.size(), 1000) < 0)
			throw std::runtime("Poll issue");

		// if ((sig_index == getPollSig()) == NO_SIGNAL)
		// 	perror("issue no signal, end server");
		// std::cout << "poll connection " << this->_fds[0].fd << std::endl;
		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			// std::cout << "fd[" << i << "].fd: " << this->_fds[i].fd << std::endl;
			// std::cout << "fd[" << i << "].events: " << this->_fds[i].events << std::endl;
			// std::cout << "fd[" << i << "].revents: " << this->_fds[i].revents << std::endl;
			// std::cout << this->_fds[i].fd << " " << this->_listening_socket << "revents" << this->_fds[i].revents << std::endl;
			if (_fds[i].fd == UNSET)
				continue;
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _listening_socket)
					acceptClient(i);
				else
					handleRequest(i);
			}
			else if (this->_fds[i].revents & POLLOUT)
				sendResponse(i);
			else if (this->_fds[i].revents & (POLLERR | POLLHUP) && _fds[i].fd != _listening_socket)
			{
				perror("error on established connection");
				close(this->_fds[i].fd);
			}
		}
	}
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

// void	ServerManager::launchServers()
// {
// 	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
// 	{
// 		const t_server& config = *it;

// 		Server server(config);
// 		server.setup();

// 		int pid = fork();
// 		if (pid == 0) {
// 			// Child process
// 			server.run();
// 			return;
// 		} else if (pid < 0) {
// 			perror("issue with the forking");
// 			//throw MyException(ErrorType::FORK, "issue when forking the server at creation");
// 			return;
// 		} else {
// 			// Parent process
// 			_childPids.push_back(pid);
// 			_servers.push_back(std::move(server));
// 		}
// 	}

// 	for (std::vector<Server>::iterator i = _servers.begin(); i != _servers.end();)
// 	{
// 		if (waitpid((*i).getPid(), NULL, 0) == -1)
// 		{
// 			perror("waitpid() failed");
// 			_servers.erase(i);
// 		}
// 		else
// 		{
// 			i++;
// 		}
// 	}
// }

// void	ServerManager::stopServers()
// {
// 	for (size_t i = 0; i < this->_servers.size(); ++i) {
// 		this->_servers[i].end();
// 	}
// }
