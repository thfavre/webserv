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
		std::string	checkFile(std::string path);
		std::string _fileContent;
		std::string	_path;
};

#endif
