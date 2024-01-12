#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<t_server> serverConfigs) : _serverConfigs(serverConfigs)
{
	// for (std::vector<t_server>::const_iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); ++it)
	// {
	// 	const t_server& config = *it;

	// 	Server server(config);
	// 	this->_fds.push_back(makeEpfd(server.getListeningSocket(), server.getName(), true));
	// 	_servers.push_back(server);
	// }

	for (size_t i = _serverConfigs.size(); i > 0; --i)
	{
		const t_server& config = _serverConfigs[i - 1];

		Server server(config);
		this->_fds.push_back(makeEpfd(server.getListeningSocket(), server.getName(), true));
		_servers.push_back(server);
	}
}

ServerManager::ServerManager(const ServerManager &src)
{
	this->_serverConfigs = src._serverConfigs;
	this->_servers = src._servers;
}

ServerManager::~ServerManager()
{
	stopServers();
	this->_serverConfigs.clear();
	this->_fds.clear();
	this->_servers.clear();
}

ServerManager	&ServerManager::operator=(const ServerManager &src)
{
	if (this != &src)
	{
		this->_serverConfigs = src._serverConfigs;
		this->_servers = src._servers;
	}
	return *this;
}

void	ServerManager::launchServers()
{
	while (true)
	{
		std::vector<pollfd> tempPollfds;
		size_t	vectorSize = this->_fds.size();

		for (size_t i = vectorSize; i > 0; --i)
		{
			size_t index = i - 1;
			pollfd tempPfd;
			tempPfd.fd = this->_fds[index].pfd.fd;  // Copy the file descriptor
			tempPfd.events = this->_fds[index].pfd.events;  // Copy other relevant fields
			tempPfd.revents = this->_fds[index].pfd.revents;
			tempPollfds.push_back(tempPfd);
		}

		printVectors(tempPollfds, this->_fds);

		if (poll(tempPollfds.data(), tempPollfds.size(), 100000) < 0)
			throw std::runtime_error("[EXCEPTION] Poll issue");

		syncFdsToTmpfds(tempPollfds);

		for (std::vector<epfd>::reverse_iterator rit = this->_fds.rbegin(); rit != this->_fds.rend(); ++rit)
		{
			size_t i = std::distance(this->_fds.begin(), rit.base()) - 1;
			Server tmpServ = getServerByName(rit->server_name);

			if (rit->pfd.revents & POLLIN)
			{
				// Handle POLLIN event
				if (rit->is_listening_socket)
				{
					// Handle listening socket event
					int client_fd = tmpServ.acceptClient(rit->pfd.fd);
					this->_fds.push_back(makeEpfd(client_fd, rit->server_name, false));
				}
				else
				{
					// Handle client socket event
					rit->response = tmpServ.handleRequest(rit->pfd.fd, &rit->keep_alive);
					if (rit->response.empty())
					{
						std::cout << "calling closeSingleSocket from POLLIN with index: " << i << std::endl;
						closeSingleSocket(rit);
					}
					else
					{
						rit->pfd.events = POLLOUT;
					}
				}
			}
			else if (rit->pfd.revents & POLLOUT)
			{
				// Handle POLLOUT event
				if ((tmpServ.sendResponse(rit->pfd.fd, rit->response) == 0) || (!rit->keep_alive))
				{
					std::cout << "calling closeSingleSocket from POLLOUT with index: " << i << std::endl;
					closeSingleSocket(rit);
				}
				else
				{
					rit->pfd.events = POLLIN;
				}
			}
			else if (rit->pfd.revents & (POLLERR | POLLHUP))
			{
				// Handle POLLERR or POLLHUP event
				std::cout << "calling closeSingleSocket from POLLERR with index: " << i << std::endl;
				closeSingleSocket(rit);
				std::cout << RED << "[ERROR] Closing socket " << i << " because of POLLERR | POLLHUP" << RESET << std::endl;
			}
		}


			// Check for events
		// for (size_t index = vectorSize; index > 0; --index)
		// {
		// 	size_t i = index - 1;
		// 	Server	tmpServ = getServerByName(this->_fds[i].server_name);
		// 	if (this->_fds[i].pfd.revents & POLLIN)
		// 	{
		// 		if (this->_fds[i].is_listening_socket == true)
		// 		{
		// 			// Handle listening socket event
		// 			int client_fd = tmpServ.acceptClient(this->_fds[i].pfd.fd);
		// 			this->_fds.push_back(makeEpfd(client_fd, this->_fds[i].server_name, false));
		// 		} else {
		// 			// Handle client socket event
		// 			//TODO: might need to close the socket if request says "close" in the header
		// 			this->_fds[i].response = tmpServ.handleRequest(this->_fds[i].pfd.fd, &this->_fds[i].keep_alive);
		// 			if (this->_fds[i].response.empty())
		// 			{
		// 				//TODO: closing the socket or closing the program in itself ??
		// 				std::cout << "calling closeSingleSocket from POLLIN with index: " << i << std::endl;
		// 				closeSingleSocket(i);
		// 				// throw std::runtime_error("Issue getting request from client"); //Not sure that it is needed to throw error for these cases
		// 			}
		// 			else
		// 			{
		// 				this->_fds[i].pfd.events = POLLOUT;
		// 			}
		// 		}
		// 	}
		// 	else if (this->_fds[i].pfd.revents & POLLOUT)
		// 	{
		// 		// Handle sending response to client
		// 		if ((tmpServ.sendResponse(this->_fds[i].pfd.fd, this->_fds[i].response) == 0) || (!this->_fds[i].keep_alive))
		// 		{
		// 			std::cout << "calling closeSingleSocket from POLLOUT with index: " << i << std::endl;
		// 			closeSingleSocket(i);
		// 		}
		// 		else
		// 		{
		// 			this->_fds[i].pfd.events = POLLIN;
		// 		}
		// 	}
		// 	else if (this->_fds[i].pfd.revents & (POLLERR | POLLHUP))
		// 	{
		// 		// Handle poll errors
		// 		std::cout << "calling closeSingleSocket from POLLERR with index: " << i << std::endl;
		// 		closeSingleSocket(i);
		// 		std::cout << RED << "[ERROR] Closing socket " << i << " because of POLLERR | POLLHUP" << RESET << std::endl;
		// 	}
		// }
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

void ServerManager::stopServers()
{
	std::vector<size_t> indicesToErase;
	checkLogs(this->_fds, "beginning stopServers");
	for (size_t i = _fds.size(); i > 0; --i)
	{
		size_t index = i - 1;
		if (this->_fds[index].pfd.fd >= 0)
		{
			// Check if the file descriptor is still valid
			if (close(this->_fds[index].pfd.fd) < 0)
			{
				// Handle the case where closing the file descriptor fails
				perror("Error closing file descriptor");
			}
			else
			{
				this->_fds[index].pfd.fd = UNSET;
			}
			indicesToErase.push_back(index);
		}
	}

	// Erase elements outside the loop to avoid iterator invalidation
	for (std::vector<size_t>::reverse_iterator it = indicesToErase.rbegin(); it != indicesToErase.rend();)
	{
		this->_fds.erase(this->_fds.begin() + *it);
		++it; // Increment the iterator after the erase operation
	}
	checkLogs(this->_fds, "end stopServers");
}

void ServerManager::closeSingleSocket(std::vector<epfd>::reverse_iterator it) {
    checkLogs(this->_fds, "beginning closeSingleSocket");
    size_t index = std::distance(it, this->_fds.rend()) - 1;

    std::cout << "closeSingleSocket index: " << index << std::endl;
    if (index >= this->_fds.size()) {
        std::cerr << RED << "[ERROR] Invalid iterator for closeSingleSocket" << RESET << std::endl;
        return;
    }

    if (it->pfd.fd >= 0) {
        std::cout << YELLOW << "[SOCKET] Closing socket " << it->pfd.fd << " at index " << index << RESET << std::endl;
        if (close(it->pfd.fd) < 0)
            std::cerr << RED << "[ERROR] when closing the fd " << it->pfd.fd << ": " << strerror(errno) << RESET << std::endl;
        it->pfd.fd = UNSET;
    }

    this->_fds.erase(it.base());
    checkLogs(this->_fds, "end closeSingleSocket");
}


// void		ServerManager::closeSingleSocket(int index)
// {
// 	if (index < 0 || static_cast<size_t>(index) >= this->_fds.size())
// 		return;
// 	if (this->_fds[index].pfd.fd >= 0)
// 	{
// 		if (close(this->_fds[index].pfd.fd) < 0)
// 			std::cerr << RED << "[ERROR] when closing the fd " << this->_fds[index].pfd.fd << RESET << std::endl;
// 		else
// 			std::cout << YELLOW << "[SOCKET] Closing socket " << index << RESET << std::endl;
// 		this->_fds[index].pfd.fd = UNSET;
// 	}
// 	this->_fds.erase(this->_fds.begin() + index);
// }


void		ServerManager::checkLogs(std::vector<struct epfd> fds, std::string where)
{
	std::cout << "-----Checking logs-----" << where << "-----" << std::endl;
	std::cout << "Size of _fds: " << this->_fds.size() << std::endl;
	for (size_t i = 0; i < fds.size(); i++)
	{
		std::cout << "FD " << fds[i].pfd.fd << ", index: " << i << std::endl;
		// std::cout << "events of the fd " << fds[i].pfd.events << std::endl;
	}
}

void		ServerManager::printVectors(const std::vector<pollfd>& vec1, const std::vector<epfd>& vec2) {
    size_t size = std::min(vec1.size(), vec2.size());

    std::cout << "tempPollfds\t\t\t_fds" << std::endl;
    for (size_t i = 0; i < size; ++i) {
        std::cout << vec1[i].fd << "\t" << vec1[i].events << "\t" << vec1[i].revents << "\t";
        std::cout << "\t" << vec2[i].pfd.fd << "\t" << vec2[i].pfd.events << "\t" << vec2[i].pfd.revents << std::endl;
    }

    // Print remaining elements in the longer vector
    for (size_t i = size; i < vec1.size(); ++i) {
        std::cout << vec1[i].fd << "\t" << vec1[i].events << "\t" << vec1[i].revents << std::endl;
    }

    for (size_t i = size; i < vec2.size(); ++i) {
        std::cout << "\t\t\t\t\t" << vec2[i].pfd.fd << "\t" << vec2[i].pfd.events << "\t" << vec2[i].pfd.revents << std::endl;
    }
}

void ServerManager::syncTmpfdsToFds(std::vector<pollfd>& tempPollfds)
{
	tempPollfds.clear();
	size_t vectorSize = this->_fds.size();
	for (size_t i = vectorSize; i > 0; --i)
	{
		size_t index = i - 1;
		tempPollfds.push_back(this->_fds[index].pfd);
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

