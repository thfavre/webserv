#include "ConfigParse.hpp"

ConfigParse::ConfigParse(void)
{
	return ;
}

ConfigParse::ConfigParse(ConfigCheck config)
{
	parseFile(config.getFileContent());
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
	_serversParsed = rhs._serversParsed;
	return *this;
}

std::vector<t_server>	ConfigParse::getServersParsed(void) const
{
	return (_serversParsed);
}

void	ConfigParse::parseFile(std::string config)
{
	std::vector<std::string>	servers_unparsed = splitServers(config);
	parseServers(servers_unparsed);
	return ;
}

std::vector<std::string>	ConfigParse::splitServers(std::string config)
{
	std::vector<std::string>	servers_unparsed;
	std::string					limit = "- server:\n";
	size_t						limit_size = limit.size();
	size_t						pos1;
	size_t						pos2;

	while ((pos1 = config.find(limit)) != std::string::npos)
	{
		if ((pos2 = config.find(limit, pos1 + 1)) != std::string::npos)
		{
			servers_unparsed.push_back(config.substr(pos1 + limit_size, pos2 - (pos1 + limit_size)));
			config = config.erase(pos1, pos2 - pos1);
		}
		else
		{
			servers_unparsed.push_back(config.substr(pos1 + limit_size));
			break;
		}
	}
	if (pos1 == std::string::npos)
		throw std::invalid_argument("Config file invalid");
	return (servers_unparsed);
}

void	ConfigParse::parseServers(std::vector<std::string> servers_unparsed)
{
	size_t	nb_servers = servers_unparsed.size();

	for (size_t i = 0; i < nb_servers; i++)
	{
		checkInfosServer(servers_unparsed[i]);
		parseInfos(servers_unparsed[i]);
	}
	return ;
}

void	ConfigParse::checkInfosServer(std::string server)
{
	size_t	pos1 = server.find("server_name: ");
	size_t	pos2 = server.find("port: ");
	size_t	pos3 = server.find("client_max_body_size: ");
	size_t	pos4 = server.find("error_pages:\n");
	size_t	pos5 = server.find("routes:\n");
	if (pos1 != std::string::npos && pos2 != std::string::npos &&
		pos3 != std::string::npos && pos4 != std::string::npos &&
		pos5 != std::string::npos && pos1 < pos2 && pos2 < pos3 &&
		pos3 < pos4 && pos4 < pos5)
		return ;
	else
		throw std::invalid_argument("Config file invalid");
	return;
}

void	ConfigParse::parseInfos(std::string server)
{
	t_server			data;
	std::string			line;
	std::istringstream	iss(server);
	bool				access_error = false;
	bool				access_routes = false;

	while (std::getline(iss, line))
	{
		if (line.find("server_name: ") != std::string::npos)
			data.server_name = parseServerName(line);
		else if (line.find("port: ") != std::string::npos)
			data.port = parsePort(line);
		else if (line.find("client_max_body_size: ") != std::string::npos)
			data.client_max_body_size = parseBodySize(line);
		else if (line.find("error_pages:\0") != std::string::npos)
			access_error = true;
		else if (line.find("routes:\0") != std::string::npos)
		{
			access_error = false;
			access_routes = true;
		}
		else if (access_error)
			parseErrorPages(line, data);
		else if (access_routes)
			parseRoutes(line, data);
	}
	_serversParsed.push_back(data);
}

std::string	ConfigParse::parseServerName(std::string line)
{
	std::string	server_name;
	std::string	key = "server_name: ";
	size_t		pos = line.find(key);
	size_t		key_size = key.size();

	server_name = line.substr(pos + key_size);
	return (server_name);
}

int		ConfigParse::parsePort(std::string line)
{
	int			port;
	std::string	key = "port: ";
	size_t		pos = line.find(key);
	size_t		key_size = key.size();
	std::string	tmp;

	tmp = line.substr(pos + key_size);
	if (areAllDigits(tmp))
		port = std::atoi(tmp.c_str());
	else
		throw std::invalid_argument("Port must consist exclusively of numbers");
	return (port);
}

size_t	ConfigParse::parseBodySize(std::string line)
{
	size_t		client_max_body_size;
	std::string	key = "client_max_body_size: ";
	size_t		pos = line.find(key);
	size_t		key_size = key.size();
	std::string	tmp;

	tmp = line.substr(pos + key_size);
	if (!std::isdigit(tmp[0]))
		throw std::invalid_argument("Client_max_body_size format is not valid");
	client_max_body_size = std::atoi(tmp.c_str());
	if (!areAllDigits(tmp))
	{
		pos = tmp.find_first_not_of("0123456789");
		if (tmp[pos + 1] != '\0')
			throw std::invalid_argument("Client_max_body_size format is not valid");
		else if ((tmp[pos] == 'k' || tmp[pos] == 'K') && client_max_body_size < 100000)
			client_max_body_size *= 1024;
		else if ((tmp[pos] == 'm' || tmp[pos] == 'M') && client_max_body_size < 100)
			client_max_body_size *= 1024 * 1024;
		else
			throw std::invalid_argument("Client_max_body_size format is not valid");
	}
	return (client_max_body_size);
}

void		ConfigParse::parseErrorPages(std::string line, t_server &data)
{
	int			key;
	std::string	value;
	std::string	tmp;
	size_t		pos1;
	size_t		pos2;

	if ((pos1 = line.find("- ")) == std::string::npos || (pos2 = line.find(": ")) == std::string::npos)
		throw std::invalid_argument("Error_pages format is not valid");
	tmp = line.substr(pos1 + 2, pos2 - (pos1 + 2));
	if (!areAllDigits(tmp))
		throw std::invalid_argument("Error_pages format is not valid");
	key = std::atoi(tmp.c_str());
	value = line.substr(pos2 + 2);
	data.error_pages.insert(std::make_pair(key, value));
}

void		ConfigParse::parseRoutes(std::string line, t_server &data)
{
	static std::string	key1;
	std::string			key2;
	std::string			value;
	size_t				delimit;

	if (line.find("- /") != std::string::npos)
		key1 = line.substr(2, line.size() - 3);
	else
	{
		if ((delimit = line.find(": ")) == std::string::npos)
			throw std::invalid_argument("Routes format is not valid");
		key2 = line.substr(0, delimit);
		value = line.substr(delimit + 2);
		data.routes[key1].insert(std::make_pair(key2, value));
	}
}

bool		ConfigParse::areAllDigits(const std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}
