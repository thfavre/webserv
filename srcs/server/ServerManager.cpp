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
	this->_serverConfigs.clear();
	this->_fds.clear();
	this->_servers.clear();
}


void	ServerManager::launchServers()
{
	while (true)
	{
		std::vector<pollfd> tempPollfds;
		size_t	vectorSize = this->_fds.size();

		for (size_t i = 0; i < vectorSize; ++i)
		{
			tempPollfds.push_back(this->_fds[i].pfd);
		}

		if (poll(tempPollfds.data(), tempPollfds.size(), 100000) < 0)
			throw std::runtime_error("[EXCEPTION] Poll issue");

			// Check for events
		for (size_t i = 0; i < vectorSize; ++i)
		{
			Server	tmpServ = getServerByName(this->_fds[i].server_name);
			if (tempPollfds[i].revents & POLLIN)
			{
				if (this->_fds[i].is_listening_socket == true)
				{
					// Handle listening socket event
					int client_fd = tmpServ.acceptClient(this->_fds[i].pfd.fd);
					this->_fds.push_back(makeEpfd(client_fd, this->_fds[i].server_name, false));
				} else {
					// Handle client socket event
					//TODO: might need to close the socket if request says "close" in the header
					this->_fds[i].response = tmpServ.handleRequest(this->_fds[i].pfd.fd, &this->_fds[i].keep_alive);
					if (this->_fds[i].response.empty())
					{
						//TODO: closing the socket or closing the program in itself ??
						closeSingleSocket(i);
						// throw std::runtime_error("Issue getting request from client"); //Not sure that it is needed to throw error for these cases
					}
					else
					{
						this->_fds[i].pfd.events = POLLOUT;
					}
				}
			}
			else if (tempPollfds[i].revents & POLLOUT)
			{
				// Handle sending response to client
				if (tmpServ.sendResponse(this->_fds[i].pfd.fd, this->_fds[i].response) == 0)
				{
					closeSingleSocket(i);
				}
				if (!this->_fds[i].keep_alive)
					closeSingleSocket(i);
				else
					this->_fds[i].pfd.events = POLLIN;
			}
			else if (tempPollfds[i].revents & (POLLERR | POLLHUP))
			{
				// Handle poll errors
				closeSingleSocket(i);
				std::cout << RED << "[ERROR] Closing socket " << i << " because of POLLERR | POLLHUP" << RESET << std::endl;
			}
		}
	}
}

Server		&ServerManager::getServerByName(const std::string &name)
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
	epfd	newEpfd(server_name, "");

	newEpfd.pfd.fd = fd;
	newEpfd.pfd.events = POLLIN;
	newEpfd.pfd.revents = 0;
	newEpfd.server_name = server_name;
	newEpfd.is_listening_socket = is_listening_socket;
	newEpfd.keep_alive = true;
	// newEpfd.response = "";

	return newEpfd;
}

void		ServerManager::stopServers()
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (this->_fds[i].pfd.fd >= 0)
		{
			// Check if the file descriptor is still valid
			if (close(this->_fds[i].pfd.fd) < 0)
			{
				// Handle the case where closing the file descriptor fails
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
		return;
	if (this->_fds[index].pfd.fd >= 0)
	{
		if (close(this->_fds[index].pfd.fd) < 0)
			std::cerr << RED << "[ERROR] when closing the fd " << this->_fds[index].pfd.fd << RESET << std::endl;
		else
			std::cout << YELLOW << "[SOCKET] Closing socket " << index << RESET << std::endl;
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
