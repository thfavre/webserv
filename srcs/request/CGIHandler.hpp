#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <string>

class CGIHandler
{
	public:
		CGIHandler(const std::string &path);
		bool isInfLoop() const;
		bool executeScript() const;
		bool isCGI() const;

	private:
		CGIHandler(const CGIHandler &other);
		CGIHandler &operator=(const CGIHandler &other);

		void _parsePath();

		std::string _path;
		std::string _extension;

		bool _isInfLoop;


};


#endif
