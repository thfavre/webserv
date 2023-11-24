#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

#include <iostream>
#include <vector>
#include <string>

class ConfigParse
{
	public :
		ConfigParse(ConfigCheck config);
		ConfigParse(ConfigParse const & src);
		~ConfigParse(void);

		ConfigParse	&operator=(ConfigParse const & rhs);

	private :
		ConfigParse(void);
		void	ParseFile(std::string config);
		std::vector<std::string>	SplitServers(std::string config);

};

#endif
