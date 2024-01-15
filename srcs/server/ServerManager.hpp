#ifndef SERVERMANAGER_CPP
# define SERVERMANAGER_CPP

#include "server.hpp"
#include <sys/wait.h>

class ServerManager
{
	struct epfd {
		pollfd		pfd;
		std::string	server_name;
		bool		is_listening_socket;
		bool		keep_alive;
		std::string	response;
	};

	std::vector<t_server>		_serverConfigs;
	std::vector<Server>			_servers;
	std::vector<struct epfd>	_fds;

	ServerManager();
	void	closeSingleSocket(int index);
	void	stopServers();
	void	syncFdsToTmpfds(const std::vector<pollfd>& tempPollfds);

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		~ServerManager();

		void		launchServers();
		epfd		makeEpfd(int fd, std::string server_name, bool is_listening_socket);
		Server		&getServerByName(std::string &name);
		void		checkLogs(std::vector<struct epfd> fds);
};

#endif
