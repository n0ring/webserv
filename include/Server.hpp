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
#include <poll.h>
#include <algorithm>
#include <vector>
#include "Cp.hpp"
#include "VHost.hpp"
#include "utils.hpp"
#include "Parser.hpp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#define NO_TIMEOUT -1


class Server {
	private: 
		// int			_listener;
	
		std::vector<VHost>	_vHosts;
		ConnectionPool							ConnectionPool;
		std::vector<pollfd>			fds;
		int							nfds;

		void			setupServers(std::string configName);
		VHost	&getVHost(int fd);
	public: 
		Server(void);
		~Server(void);

		bool 			isFdListener(int fd); // to private
		void			start(std::string configName);
};