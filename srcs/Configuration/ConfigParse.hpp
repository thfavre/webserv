#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include "ConfigCheck.hpp"

typedef struct s_server
{
	std::string	server_name;
	std::string	port; // utilisation en string ou int?
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
		std::vector<t_server>		ParseServers(std::vector<std::string> ServersUnparsed);
		std::vector<std::string>	SplitServers(std::string config);
		t_server					ParseInfo(std::string ServersUnparsed);
		std::vector<t_server>		_ServersParsed;

};

#endif
