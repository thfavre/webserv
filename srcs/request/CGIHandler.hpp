#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <string>

# define MAX_CGI_EXECUTION_TIME 3 // seconds

class CGIHandler
{
	public:
		CGIHandler(const std::string &path);
		bool executeScript(std::string CGIPath) const;
		bool isInfLoop() const;
		bool isCGI() const;
		const std::string getExtension() const;
		std::string getScriptExecutionOutput() const;

		CGIHandler &operator=(const CGIHandler &other);

	private:
		// CGIHandler(const CGIHandler &other);

		void _parsePath();

		std::string _path;
		std::string _extension;

		mutable bool _isInfLoop;
		mutable std::string _output;


};


#endif
