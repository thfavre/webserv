#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

class ConfigCheck
{
	public :
		ConfigCheck(std::string path);
		ConfigCheck(ConfigCheck const & src);
		~ConfigCheck(void);

		ConfigCheck &operator=(ConfigCheck const & rhs);

		std::string	getFileContent(void) const;


	private :
		ConfigCheck(void);
		std::string	CheckFile(std::string path);
		std::string _FileContent;
		std::string	_Path;
};

#endif
