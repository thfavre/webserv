#include "ConfigCheck.hpp"

ConfigCheck::ConfigCheck(void)
{
	return ;
}

ConfigCheck::ConfigCheck(std::string path) : _path(path)
{
	_ContentFile = CheckFile(path);
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
	// TO COMPLETE
	return *this;
}


