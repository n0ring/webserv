#pragma once

#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <algorithm>

#include "Server.hpp"

class Server;

class Select
{
private:
	Server &_server;
	fd_set	masterFd;
	fd_set	readFd;
	fd_set	writeFd;


public:
	Select(Server &server);
	~Select();

	void	launch(void);
	void	setNewConnection(int &fdmax);
};
