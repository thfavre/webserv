#ifndef SERVERPARSED_HPP
# define SERVERPARSED_HPP

#include <iostream>
#include <vector>
#include <string>
#include "ConfigParse.hpp"

class ServerParsed
{
	public :
		ServerParsed(std::vector<std::string> ServersUnparsed);
		ServerParsed(ServerParsed const & src);
		~ServerParsed(void);

		Serverparsed &operator=(ServerParsed const & rhs);

	private :
		ServerParsed(void);
};

#endif
