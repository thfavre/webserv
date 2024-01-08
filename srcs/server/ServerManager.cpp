#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{
	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	{
		const t_server& config = *it;

		Server server(config);
		this->_fds.push_back({{server.getListeningSocket(), POLLIN, 0}, server.getName(), true});
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

		std::vector<pollfd> tempPollfds;
		for (const auto& epfd : this->_fds)
		{
			tempPollfds.push_back(epfd.pfd);
		}

		if (poll(tempPollfds.data(), tempPollfds.size(), 1000) < 0)
			throw std::runtime_error("Poll issue");

			// Check for events
		for (size_t i = 0; i < tempPollfds.size(); ++i)
		{
			if (tempPollfds[i].revents & POLLIN) {
				Server	tmpServ = getServerByName(this->_fds[i].server_name);
				if (this->_fds[i].is_listening_socket == true) {
					// Accept new connection on this server
					int client_fd = tmpServ.acceptClient(tempPollfds[i].fd);
					// Create and add a new epfd for the client socket
					this->_fds.push_back({{client_fd, POLLIN, 0}, this->_fds[i].server_name, false});
				} else {
					// Handle client socket event
					// Use fds[i].server_name to determine the associated server
				}
			}
		}

		// if ((sig_index == getPollSig()) == NO_SIGNAL)
		// 	perror("issue no signal, end server");
		// std::cout << "poll connection " << this->_fds[0].fd << std::endl;
		// for (int i = 0; i < MAX_CONNECTION; i++)
		// {
		// 	// std::cout << "fd[" << i << "].fd: " << this->_fds[i].fd << std::endl;
		// 	// std::cout << "fd[" << i << "].events: " << this->_fds[i].events << std::endl;
		// 	// std::cout << "fd[" << i << "].revents: " << this->_fds[i].revents << std::endl;
		// 	// std::cout << this->_fds[i].fd << " " << this->_listening_socket << "revents" << this->_fds[i].revents << std::endl;
		// 	if (_fds[i].fd == UNSET)
		// 		continue;
		// 	if (_fds[i].revents & POLLIN)
		// 	{
		// 		if (_fds[i].fd == _listening_socket)
		// 			acceptClient(i);
		// 		else
		// 			handleRequest(i);
		// 	}
		// 	else if (this->_fds[i].revents & POLLOUT)
		// 		sendResponse(i);
		// 	else if (this->_fds[i].revents & (POLLERR | POLLHUP) && _fds[i].fd != _listening_socket)
		// 	{
		// 		perror("error on established connection");
		// 		close(this->_fds[i].fd);
		// 	}
		// }
	}
}

Server		&ServerManager::getServerByName(std::string &name)
{
	for (std::vector<Server>::iterator	it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	throw std::runtime_error("Can not find the server by its name");
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
