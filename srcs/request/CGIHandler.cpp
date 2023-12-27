#include "CGIHandler.hpp"
#include <unistd.h>
#include <iostream>

CGIHandler::CGIHandler(const std::string &path) : _path(path)
{
	_isInfLoop = false;
	_parsePath();
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
		execve(_path.c_str(), NULL, NULL); // ! TODO set env and agrs variables
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else // parent
	{
		// close write end of pipe
		close(pipefd[1]);

		// read from pipe
		char buffer[4096];
		int readBytes;
		time_t start = time(NULL);
		while ((readBytes = read(pipefd[0], buffer, 4096)) > 0)
		{
			buffer[readBytes] = '\0';
			std::cout << buffer;
		}
		if (readBytes == -1)
		{
			perror("read");
			return false;
		}

		// close read end of pipe
		close(pipefd[0]);

		// wait for child to finish
		// if child is in infinite loop, kill it (> Xms)
		int status;
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			return false;
		}
		if (WIFEXITED(status))
		{
			std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
		}
		else if (WIFSIGNALED(status))
		{
			std::cout << "Child was killed by signal " << WTERMSIG(status) << std::endl;
		}
		else if (WIFSTOPPED(status))
		{
			std::cout << "Child was stopped by signal " << WSTOPSIG(status) << std::endl;
		}
		else if (WIFCONTINUED(status))
		{
			std::cout << "Child was continued" << std::endl;
		}
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
