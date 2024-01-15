#include <iostream>
#include "request/HTTPRequest.hpp"
#include "server/server.hpp"
#include "server/ServerManager.hpp"
#include "Configuration/ConfigCheck.hpp"
#include "Configuration/ConfigParse.hpp"

void httpRequestTest();

int main(int ac, char **av)
{
	std::string config_file;

	if (ac == 1)
		config_file = "config.yaml"; // default config file
	else if (ac == 2)
		config_file = av[1];
	else
	{
		std::cout << "Invalid number of arguments, please use: ./webserv [config_file]" << std::endl;
		return 1;
	}
	try {
		ConfigCheck		config(config_file);
		ConfigParse		parse(config);
		ServerManager	serverManager(parse.getServersParsed());
		serverManager.launchServers();
	} catch (std::exception &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
	return 0;
}
