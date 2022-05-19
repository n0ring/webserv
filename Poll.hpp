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

class Poll
{
private:
	Server &_server;
	std::vector<pollfd> fds;
	std::vector<pollfd> outfds;
	int					nfds;
	int					timeout;


	void	setNewConnection();
	void	handleExistConnection(pollfd &fdToHandle);
	void	sendToAll(char *buf, int author);
	pollfd	make_fd(int fd, int event);
	void	deleteFd(int fdToDel);
public:
	Poll(Server &server);
	~Poll();

	void launch(void);
};
