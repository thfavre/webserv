#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

class ConfigParse
{
	public :
		ConfigParse(ConfigCheck config);
		ConfigParse(ConfigParse const & src);
		~ConfigParse(void);

		ConfigParse	&operator=(ConfigParse const & rhs);

	private :
		ConfigParse(void);
};

#endif
