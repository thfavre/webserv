#include <iostream>
#include "request/HTTPRequest.hpp"
#include "server/server.hpp"
#include "server/ServerManager.hpp"
#include "Configuration/ConfigCheck.hpp"
#include "Configuration/ConfigParse.hpp"
#include "exception/exceptions.hpp"

void httpRequestTest();

int main(int ac, char **av)
{
	(void) av;
	if (ac != 2)
	{
		std::cout << "Invalid number of arguments, please provide a configuration file" << std::endl;
		return 1;
	}
	try {
		ConfigCheck		config(av[1]);
		ConfigParse		parse(config);
		ServerManager	serverManager(parse.getServersParsed());
		serverManager.launchServers();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	//Work in progress for exceptions
	// catch (const MyException& e) {
	// 	std::cerr << "Exception of type " << static_cast<int>(e.getType())
	// 				<< " caught: " << e.what() << std::endl;
	// }
	return 0;
}
