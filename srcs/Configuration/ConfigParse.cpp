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
	std::vector<t_server>	ServersParsed = ParseServers(ServersUnparsed);
	return (ServersParsed);
}


std::vector<std::string>	ConfigParse::SplitServers(std::string config)
{
	std::vector<std::string> ServersUnparsed;
	std::string	limit = "- server:\n";
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
	if (pos == std::string::npos)
		throw std::invalid_argument("Config file invalid");
	return ServersUnparsed;
}

std::vector<t_server>	ConfigParse::ParseServers(std::vector<std::string> ServersUnparsed)
{
	std::vector<t_server>	ServersParsed;
	size_t	NbServers = ServersUnparsed.size();

	for (int i = 0; i < NbServers; i++)
	{
		CheckInfosServer(ServersUnparsed[i]);
		t_server	data;
		data = ParseInfo(ServersUnparsed[i], data);
		ServersParsed.push_back(data);
	}
	return (ServersParsed);
}

void		ConfigParse::CheckInfosServer(std::string server)
{
	size_t	pos0 = server.find("server_name:");
	size_t	pos1 = server.find("port:");
	size_t	pos2 = server.find("client_max_body_size:");
	size_t	pos3 = server.find("error_pages:");
	size_t	pos4 = server.find("routes:");
	if (pos0 != std::string::npos && pos1 != std::string::npos &&
		pos2 != std::string::npos && pos3 != std::string::npos &&
		pos4 != std::string::npos && pos0 < pos1 && pos1 < pos2 &&
		pos2 < pos3 && pos3 < pos4)
		return ;
	else
		throw std::invalid_argument("Config file invalid");
	return;
}

t_server	ConfigParse::ParseInfo(std::string server, t_server &data)
{
	std::string	NewStr = server;

	NewStr = ParseServerName(NewStr, data);
	NewStr = ParsePort(NewStr, data);
	NewStr = ParseBodySize(NewStr, data);
	NewStr = ParseErrorPages(NewStr, data);
	std::cout << "THIS IS A SERVER" << std::endl;
	std::cout << data.server_name << std::endl;
	std::cout << data.port << std::endl;
	std::cout << data.client_max_body_size << std::endl;
	std::cout << NewStr << std::endl;
	return (data);
}

std::string		ConfigParse::ParseServerName(std::string NewStr, t_server &data)
{
	size_t	start;
	size_t	end;
	std::string	limit = "server_name: ";
	size_t	limitSize = limit.size();

	if ((start = NewStr.find(limit)) != std::string::npos &&
		((end = NewStr.find("\n")) != std::string::npos))
	{
		data.server_name = NewStr.substr(start + limitSize, end - (start + limitSize));
		NewStr = NewStr.erase(0, end + 1);
	}
	else
		throw std::invalid_argument("Server_name is invalid during parsing");
	return (NewStr);
}

std::string		ConfigParse::ParsePort(std::string NewStr, t_server &data)
{
	size_t	start;
	size_t	end;
	std::string	limit = "port: ";
	std::string	tmp;
	size_t	limitSize = limit.size();

	if ((start = NewStr.find(limit)) != std::string::npos &&
		((end = NewStr.find("\n")) != std::string::npos))
	{
		tmp = NewStr.substr(start + limitSize, end - (start + limitSize));
		if (areAllDigits(tmp))
		{
			data.port = std::atoi(tmp.c_str());
			NewStr = NewStr.erase(0, end + 1);
		}
		else
			throw std::invalid_argument("Port is invalid during parsing");
	}
	else
		throw std::invalid_argument("Port is invalid during parsing");
	return (NewStr);
}

std::string		ConfigParse::ParseBodySize(std::string NewStr, t_server &data)
{
	size_t	start;
	size_t	end;
	std::string	limit = "client_max_body_size: ";
	std::string	tmp;
	size_t	limitSize = limit.size();

	if ((start = NewStr.find(limit)) != std::string::npos &&
		((end = NewStr.find("\n")) != std::string::npos))
	{
		tmp = NewStr.substr(start + limitSize, end - (start + limitSize));
		data.client_max_body_size = std::atoi(tmp.c_str());
		if (!areAllDigits(tmp))
		{
			if (((tmp.find("k") != std::string::npos || tmp.find("K") != std::string::npos)) && data.client_max_body_size < 100000)
				data.client_max_body_size *= 1024;
			else if (((tmp.find("m") != std::string::npos || tmp.find("M") != std::string::npos)) && data.client_max_body_size < 100)
				data.client_max_body_size *= 1024 * 1024;
			else
				throw std::invalid_argument("Client_max_body_size is invalid during parsing");
		}
		NewStr = NewStr.erase(0, end + 1);
	}
	else
		throw std::invalid_argument("Client_max_body_size is invalid during parsing");
	return (NewStr);
}

std::string		ConfigParse::ParseErrorPages(std::string NewStr, t_server &data)
{

	return (NewStr);
}

bool		ConfigParse::areAllDigits(const std::string& str) // Norme 98 pas respectée
{
	return std::all_of(str.begin(), str.end(), [](unsigned char c)
	{
		return std::isdigit(c) != 0;
	});
}
