#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <algorithm>
#include <cctype>
#include "ConfigCheck.hpp"

typedef struct s_server
{
	std::string													server_name;
	int															port;
	size_t														client_max_body_size;
	std::map<int, std::string> 									error_pages;
	std::map<std::string, std::map<std::string, std::string> >	routes;
}	t_server;

class ConfigParse
{
	public :
		ConfigParse(ConfigCheck config);
		ConfigParse(ConfigParse const & src);
		~ConfigParse(void);

		ConfigParse	&operator=(ConfigParse const & rhs);

		std::vector<t_server>	getServersParsed(void) const;

	private :
		ConfigParse(void);
		void						parseFile(std::string config);
		std::vector<std::string>	splitServers(std::string config);
		void						parseServers(std::vector<std::string> servers_unparsed);
		void						checkInfosServer(std::string server);
		void						parseInfos(std::string server);
		std::string					parseServerName(std::string line);
		int							parsePort(std::string line);
		size_t						parseBodySize(std::string line);
		void						parseErrorPages(std::string line, t_server &data);
		void						parseRoutes(std::string line, t_server &data);
		bool						areAllDigits(const std::string& str);
		std::vector<t_server>		_serversParsed;
};

#endif
