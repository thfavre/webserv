#include "ConfigCheck.hpp"

ConfigCheck::ConfigCheck(void)
{
	return ;
}

ConfigCheck::ConfigCheck(std::string path) : _Path(path)
{
	_FileContent = CheckFile(_Path);
	std::cout << _FileContent << std::endl;
	return ;
}

ConfigCheck::ConfigCheck(ConfigCheck const & src)
{
	*this = src;
	return ;
}

ConfigCheck::~ConfigCheck(void)
{
	return ;
}

ConfigCheck	&ConfigCheck::operator=(ConfigCheck const & rhs)
{
	_FileContent = rhs._FileContent;
	_Path = rhs._Path;
	return *this;
}

std::string	ConfigCheck::getFileContent(void) const
{
	return (_FileContent);
}

std::string	ConfigCheck::CheckFile(std::string path)
{
	if (path.empty())
		throw std::invalid_argument("File path does not exist");
	if ((access(path.c_str(), F_OK) != 0))
		throw std::invalid_argument("File does not exist");
	std::ifstream	file(path.c_str());
	if (!file.is_open())
		throw std::invalid_argument("File can not be opened");

std::stringstream buffer;
std::string line;

	while(std::getline(file, line))
	{
		line = line.erase(line.find_last_not_of(" \n") + 1);
		if (line.find_first_not_of(" \n") != std::string::npos)
		{
			line = line.substr(line.find_first_not_of(" \n"));
			buffer << line << "\n";
		}
	}
	if (!file.eof())
		throw std::ifstream::failure("Error reading file");
	file.close();
	if (file.is_open())
		throw std::ifstream::failure("Error closing file");
return (buffer.str());
}

