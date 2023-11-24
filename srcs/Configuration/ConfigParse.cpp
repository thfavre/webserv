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
	std::vector<std::string> ServersUnparsed = SplitServers(config);
}

std::vector<std:string>	ConfigParse::SplitServers(std::string config)
{
	std::vector<std::string> ServersUnparsed;
	std::string	limit = "- server:";
	size_t	limitSize = limit.size();

	while ((pos = config.find(limit)) != std::string::npos)
	{
		if (NextPos = config.find(limit, pos + 1) != std::string::npos)
		{

		}
		else
		{
			ServersUnparsed.push_back()
		}
	}
	return ServersUnparsed;
}
