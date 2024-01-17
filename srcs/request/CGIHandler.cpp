#include "CGIHandler.hpp"
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

CGIHandler::CGIHandler(const std::string &path) : _path(path), _isInfLoop(false)
{
	_parsePath();
}

CGIHandler::CGIHandler(const std::string &path, std::list<std::string> args) : _path(path), _args(args), _isInfLoop(false)
{
	_parsePath();
}


// CGIHandler &CGIHandler::operator=(const CGIHandler &other)
// {
// 	if (this != &other)
// 	{
// 		_path = other._path;
// 		_extension = other._extension;
// 		_isInfLoop = other._isInfLoop;
// 	}
// 	return *this;
// }

void CGIHandler::_parsePath()
{
	std::string::size_type dotIndex = _path.find_last_of('.');
	if (dotIndex == std::string::npos)
		return ;

	_extension = _path.substr(dotIndex + 1);
}

bool CGIHandler::executeScript(std::string CGIPath) const
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

		char *argv[] = {};
		argv[0] = (char*)(CGIPath.c_str());
		argv[1] = (char*)(_path.c_str());
		// argv[2] = NULL;
		int index = 2;
		for (std::list<std::string>::const_iterator it = _args.begin(); it != _args.end(); it++)
		{
			argv[index] = (char*)(it->c_str());
			index++;
		}
		argv[index] = NULL;
		execve(argv[0], argv, NULL); // ! TODO set env and agrs variables
		perror("execve");
		std::cerr << "execve failed" << std::endl;

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
}

bool CGIHandler::isCGI() const
{
	if (_extension == "py")
		return true;
	if (_extension == "pl")
		return true;
	return false;
}

const std::string CGIHandler::getExtension() const
{
	return _extension;
}

bool CGIHandler::isInfLoop() const
{
	return _isInfLoop;
}

std::string CGIHandler::getScriptExecutionOutput() const
{
	return _output;
}
