#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>

class ConfigCheck
{
	public :
		ConfigCheck(std::string path);
		ConfigCheck(ConfigCheck const & src);
		~ConfigCheck(void);

		ConfigCheck &operator=(ConfigCheck const & rhs);



	private :
		ConfigCheck(void);
		std::string _ContentFile;
		std::string	_path;
};

#endif
