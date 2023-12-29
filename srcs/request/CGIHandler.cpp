#include "CGIHandler.hpp"
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>

CGIHandler::CGIHandler(const std::string &path) : _path(path), _isInfLoop(false)
{
	_parsePath();
}


CGIHandler &CGIHandler::operator=(const CGIHandler &other)
{
	if (this != &other)
	{
		_path = other._path;
		_extension = other._extension;
		_isInfLoop = other._isInfLoop;
	}
	return *this;
}

void CGIHandler::_parsePath()
{
	std::string::size_type dotIndex = _path.find_last_of('.');
	if (dotIndex == std::string::npos)
		return ;

	_extension = _path.substr(dotIndex + 1);
}

bool CGIHandler::executeScript() const
{
	int	pid;
	int	pipefd[2]; // pipefd[0] is for reading, pipefd[1] is for writing

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return false;
	}


	// create child
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return false;
	}
	else if (pid == 0) // child
	{
		// close read end of pipe
		close(pipefd[0]);
		// redirect stdout to pipe
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			close(pipefd[1]);
			return false;
		}
		close(pipefd[1]);
		// execute script
		std::string _root = "./"; // TODO come from config parser
		char *path = (char *)(_root + _path).c_str();


		char *argv[] = {};
		argv[0] = (char*)"/usr/bin/python3"; // ! TODO come from config
		argv[1] = path;
		argv[2] = NULL;
		std::cerr << "path: " << path << std::endl;
		std::cerr << "argv[0 ]: " << argv[0] << std::endl;
		execve(argv[0], argv, NULL); // ! TODO set env and agrs variables
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else // parent
	{
		// close write end of pipe
		close(pipefd[1]);
		time_t start = time(NULL);

		pid_t wpid;
		while ((wpid = waitpid(pid, NULL, WNOHANG)) <= 0)
		{
			if (wpid == -1)
			{
				perror("waitpid");
				close(pipefd[0]);
				return false;
			}
			if (wpid == 0 and start + MAX_CGI_EXECUTION_TIME < time(NULL))
			{
				_isInfLoop = true;
				kill(pid, SIGKILL);
				close(pipefd[0]);
				return false;
			}
		}

		// read from pipe
		char buffer[4096];
		int readBytes;
		while ((readBytes = read(pipefd[0], buffer, 4096)) > 0)
		{
			std::cout << "readBytes: " << readBytes << std::endl;
			std::cout << "buffer: " << buffer << std::endl;
			_output += std::string(buffer, readBytes);
		}
		close(pipefd[0]);
		if (readBytes == -1)
		{
			perror("read");
			return false;
		}
		return true;
	}

	// execute script
	// communicate with child through pipes

	// wait for child to finish
	// if child is in infinite loop, kill it (> Xms)

}

bool CGIHandler::isCGI() const
{
	if (_extension == "py")
		return true;
	return false;
}

bool CGIHandler::isInfLoop() const
{
	return _isInfLoop;
}

std::string CGIHandler::getScriptExecutionOutput() const
{
	return _output;
}
