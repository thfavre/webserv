#include "ConfigParse.hpp"

ConfigParse::ConfigParse(void)
{
	return ;
}

ConfigParse::ConfigParse(ConfigCheck config)
{
	_ServersParsed = ParseFile(config.getFileContent());
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

std::vector<t_server>	ConfigParse::ParseFile(std::string config)
{
	std::vector<std::string>	ServersUnparsed = SplitServers(config);
	for (const std::string& server : ServersUnparsed)
	{
		std::cout << "This is a server" << std::endl;
		std::cout << server << std::endl;
	}
	std::vector<t_server>	ServersParsed = ParseServers(ServersUnparsed);
	return (ServersParsed);
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

std::vector<t_server>	ConfigParse::ParseServers(std::vector<std::string> ServersUnparsed)
{
	std::vector<t_server>	ServersParsed;
	size_t	NbServers = ServersUnparsed.size();

	for (int i = 0; i < NbServers; i++)
	{
		t_server	data;
		// Check error?
		data = ParseInfo(ServersUnparsed[i]);
		ServersParsed.push_back(data);
	}
	return (ServersParsed);
}

t_server	ConfigParse::ParseInfo(std::string ServersUnparsed)
{

}
