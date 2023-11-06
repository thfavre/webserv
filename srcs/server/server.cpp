#include "server.hpp"

Server::Server()
{

}

Server::Server(int port) : _port(port)
{

}

Server::~Server()
{

}

void	Server::setup()
{
	/*
	Not sure if this will be infos I get from the parsing or not
	*/
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = this->_port;

	if (bind(this->_socket, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << this->_port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::listen_connection()
{
	/*
	Probably need to make another class that will take into account all of the incoming and dispatch them in different fd to the server
	*/
	if (listen(this->_socket, 10) < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::handle_request(int client_request)
{
	/*
	Might be something to do in the parser and then execute from here
	*/
}

void	Server::accept_connection()
{
	int	addrlen = sizeof(this->_sockaddr);
	int	connection = accept(this->_socket, (struct sockaddr*)&this->_sockaddr, (socklen_t*)&addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	/*
	Handle the read of the connection and what needs to be done
	*/
	std::string	response = "Here is the ansswer\n";
	send(connection, response.c_str(), response.size(), 0);

	this->close(connection);
}

void	Server::close(int connection)
{
	close(connection);
	close(this->_socket);
}
