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
#include <map>
#include "Server.hpp"
#include "Connection.hpp"

class Server;

class Poll
{
	private:
		Server&						_server;
		std::vector<pollfd>			fds;
		std::map<int, Connection>	_connections;
		int							nfds;
		int							timeout;



		void	setNewConnection(pollfd& listenerPollfd);
		void	handleExistConnection(int index);
		pollfd	make_fd(int fd, int event);
		void	removeConnection(int fd);
		void	addConnection(int fd, int listener);
		void	removeUselessFd(void);
	public:
		Poll(Server &server);
		~Poll();

		void launch(void);
};
