#include <fstream>

bool checkFileExists(const std::string &path)
{
	std::ifstream file(path.c_str());
	return (file.good());
}
