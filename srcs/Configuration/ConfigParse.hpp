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
	std::string	server_name;
	int	port;
	std::string	client_max_body_size; // Convertir?
	std::map<int, std::string> error_pages;
	std::map<std::string, std::map<std::string, std::string> > routes;
}	t_server;

class ConfigParse
{
	public :
		ConfigParse(ConfigCheck config);
		ConfigParse(ConfigParse const & src);
		~ConfigParse(void);

		ConfigParse	&operator=(ConfigParse const & rhs);

	private :
		ConfigParse(void);
		std::vector<t_server>		ParseFile(std::string config);
		std::vector<std::string>	SplitServers(std::string config);
		std::vector<t_server>		ParseServers(std::vector<std::string> ServersUnparsed);
		void						CheckInfosServer(std::string server);
		t_server					ParseInfo(std::string Server, t_server &data);
		std::string					ParseServerName(std::string NewStr, t_server &data);
		std::string					ParsePort(std::string NewStr, t_server &data);
		std::string					ParseBodySize(std::string NewStr, t_server &data);
		bool						areAllDigits(const std::string& str);
		std::vector<t_server>		_ServersParsed;

};

#endif
