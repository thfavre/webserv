#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <string>
# include <list>

# define MAX_CGI_EXECUTION_TIME 3 // seconds

class CGIHandler
{
	public:
		CGIHandler(const std::string &path);
		CGIHandler(const std::string &path, std::list<std::string> args);
		bool executeScript(std::string CGIPath) const;
		bool isInfLoop() const;
		bool isCGI() const;
		const std::string getExtension() const;
		std::string getScriptExecutionOutput() const;

	private:

		void _parsePath();

		std::string _path;
		std::string _extension;
		std::list<std::string> _args;

		mutable bool _isInfLoop;
		mutable std::string _output;


};


#endif
