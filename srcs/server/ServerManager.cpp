#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{
	for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	{
		const t_server& config = *it;

		Server server(config);
		this->_fds.push_back(makeEpfd(server.getListeningSocket(), server.getName(), true));
		_servers.push_back(server);
	}
}

ServerManager::~ServerManager()
{
	stopServers();
}


void	ServerManager::launchServers()
{
	while (true)
	{
		std::vector<pollfd> tempPollfds;
		for (std::vector<epfd>::const_iterator it = this->_fds.begin(); it != this->_fds.end(); ++it)
		{
			tempPollfds.push_back(it->pfd);
		}

		if (poll(tempPollfds.data(), tempPollfds.size(), 1000) < 0)
			throw std::runtime_error("[EXCEPTION] Poll issue");

		syncFdsToTmpfds(tempPollfds);

		for (size_t i = 0; i < this->_fds.size(); ++i)
		{
			Server	tmpServ = getServerByName(this->_fds[i].server_name);
			if (this->_fds[i].pfd.revents & POLLIN)
			{
				if (this->_fds[i].is_listening_socket == true)
				{
					// Handle listening socket event
					int client_fd = tmpServ.acceptClient(this->_fds[i].pfd.fd);
					this->_fds.push_back(makeEpfd(client_fd, this->_fds[i].server_name, false));
				} else {
					// Handle client socket event
					this->_fds[i].response = tmpServ.handleRequest(this->_fds[i].pfd.fd, &this->_fds[i].keep_alive);
					if (this->_fds[i].response.empty())
					{
						closeSingleSocket(i);
					}
					else
					{
						this->_fds[i].pfd.events = POLLOUT;
					}
				}
			}
			else if (this->_fds[i].pfd.revents & POLLOUT)
			{
				// Handle sending response to client
				if (tmpServ.sendResponse(this->_fds[i].pfd.fd, this->_fds[i].response) == 0)
				{
					closeSingleSocket(i);
				}
				if (!this->_fds[i].keep_alive)
				{
					closeSingleSocket(i);
				}
				else
					this->_fds[i].pfd.events = POLLIN;
			}
			else if (this->_fds[i].pfd.revents & (POLLERR | POLLHUP))
			{
				// Handle poll errors
				closeSingleSocket(i);
				std::cout << RED << "[ERROR] Closing socket " << i << " because of POLLERR | POLLHUP" << RESET << std::endl;
			}
		}
		tempPollfds.clear();
	}
}

Server		&ServerManager::getServerByName(std::string &name)
{
	for (std::vector<Server>::iterator	it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	throw std::runtime_error("[EXCEPTION] Can not find the server by its name");
}

ServerManager::epfd		ServerManager::makeEpfd(int fd, std::string server_name, bool is_listening_socket)
{
	epfd	newEpfd;

	newEpfd.pfd.fd = fd;
	newEpfd.pfd.events = POLLIN;
	newEpfd.pfd.revents = 0;
	newEpfd.server_name = server_name;
	newEpfd.is_listening_socket = is_listening_socket;
	newEpfd.keep_alive = true;
	newEpfd.response = "";

	return newEpfd;
}

void		ServerManager::stopServers()
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (this->_fds[i].pfd.fd >= 0)
		{
			if (close(this->_fds[i].pfd.fd) < 0)
			{
				perror("Error closing file descriptor");
			}
			else
			{
				this->_fds[i].pfd.fd = UNSET;
			}
		}
	}
}

void		ServerManager::closeSingleSocket(int index)
{
	if (index < 0 || static_cast<size_t>(index) >= this->_fds.size())
	{
		std::cerr << RED << "[ERROR] Socket not in range to be closed" << RESET << std::endl;
		return;
	}
	if (this->_fds[index].pfd.fd >= 0)
	{
		std::cout << YELLOW << "[SOCKET] Closing socket " << this->_fds[index].pfd.fd << RESET << std::endl;
		if (close(this->_fds[index].pfd.fd) < 0)
			std::cerr << RED << "[ERROR] when closing the fd " << this->_fds[index].pfd.fd << RESET << std::endl;
		this->_fds[index].pfd.fd = UNSET;
	}
	this->_fds.erase(this->_fds.begin() + index);
}


void		ServerManager::checkLogs(std::vector<struct epfd> fds)
{
	std::cout << "-----Checking logs-----" << std::endl;
	for (size_t i = 0; i < fds.size(); i++)
	{
		std::cout << "FD " << fds[i].pfd.fd << " (listening: " << fds[i].is_listening_socket << ", name: " << fds[i].server_name << std::endl;
		std::cout << "events of the fd " << fds[i].pfd.events << std::endl;
	}
}

void ServerManager::syncFdsToTmpfds(const std::vector<pollfd>& tempPollfds)
{
	size_t tmpSize = std::min(this->_fds.size(), tempPollfds.size());
	for (size_t i = 0; i < tmpSize; ++i)
	{
		this->_fds[i].pfd.events = tempPollfds[i].events;
		this->_fds[i].pfd.revents = tempPollfds[i].revents;
	}
}
