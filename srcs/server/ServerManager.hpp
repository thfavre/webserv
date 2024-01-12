#ifndef SERVERMANAGER_CPP
# define SERVERMANAGER_CPP

#include "server.hpp"
#include "../exception/exceptions.hpp"
#include <sys/wait.h>

class ServerManager
{
	struct epfd {
		pollfd		pfd;
		std::string	server_name;
		bool		is_listening_socket;
		bool		keep_alive;
		std::string	response;

		epfd(std::string name, std::string response)
			: pfd(), server_name(name), is_listening_socket(false), keep_alive(true), response(response) {}
		~epfd() {
			server_name.clear();
			response.clear();
		}
	};

	std::vector<t_server>		_serverConfigs;
	std::vector<Server>			_servers;
	std::vector<struct epfd>	_fds;

	ServerManager();
	void	closeSingleSocket(std::vector<epfd>::reverse_iterator it);
	void	stopServers();
	void	syncTmpfdsToFds(std::vector<pollfd>& tempPollfds);
	void	syncFdsToTmpfds(const std::vector<pollfd>& tempPollfds);

	public:
		ServerManager(std::vector<t_server> serverConfigs);
		ServerManager(const ServerManager &src);
		~ServerManager();
		ServerManager	&operator=(const ServerManager &src);

		void			launchServers();
		epfd			makeEpfd(int fd, std::string server_name, bool is_listening_socket);
		Server			&getServerByName(const std::string &name);
		void			checkLogs(std::vector<struct epfd> fds, std::string where);
		void			printVectors(const std::vector<pollfd>& vec1, const std::vector<epfd>& vec2);
};

#endif
