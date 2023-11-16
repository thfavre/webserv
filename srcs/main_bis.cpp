#include <iostream>

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
	std::cerr << "Please put only one argument [configuration file]." << std::endl;
	return EXIT_FAILURE;
	}
	try
	{
		blabla
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
