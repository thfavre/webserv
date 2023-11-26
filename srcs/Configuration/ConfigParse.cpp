#include "ConfigParse.hpp"

ConfigParse::ConfigParse(void)
{
	return ;
}

ConfigParse::ConfigParse(ConfigCheck config)
{
	ParseFile(config.getFileContent());
	return ;
}

ConfigParse::ConfigParse(ConfigParse const & rhs)
{
	*this = rhs;
	return ;
}

ConfigParse::~ConfigParse(void)
{
	return ;
}

ConfigParse	&ConfigParse::operator=(ConfigParse const & rhs)
{
	// to do
	return *this;
}

void	ConfigParse::ParseFile(std::string config)
{
	std::vector<std::string>	ServersUnparsed = SplitServers(config);
	for (const std::string& server : ServersUnparsed)
	{
		std::cout << "This is a server" << std::endl;
		std::cout << server << std::endl;
	}
	std::vector<ServerParsed>	ServerParsed(ServersUnparsed); // Do Next - Est-ce correct?
}

std::vector<std::string>	ConfigParse::SplitServers(std::string config)
{
	std::vector<std::string> ServersUnparsed;
	std::string	limit = "- server:";
	size_t	pos;
	size_t	NextPos;
	size_t	limitSize = limit.size();

	while ((pos = config.find(limit)) != std::string::npos)
	{
		if ((NextPos = config.find(limit, pos + 1)) != std::string::npos)
		{
			ServersUnparsed.push_back(config.substr(pos + limitSize, NextPos - (pos + limitSize)));
			config = config.erase(pos, NextPos - pos);
		}
		else
		{
			ServersUnparsed.push_back(config.substr(pos + limitSize));
			break;
		}
	}
	return ServersUnparsed;
}
