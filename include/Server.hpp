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
#include "Poll.hpp"
#include "ServerConfig.hpp"
#include "utils.hpp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

class Poll;
class Select;
class Server {
	private: 
		// int			_listener;
	
		std::vector<ServerConfig>	_configs;
		std::vector<int>			_listeners;

	
	public: 
		Server(void);
		~Server(void);

		void			start(void);
		void			setListenersPoll(std::vector<pollfd> &v);

		bool 			isFdListener(int fd);
		ServerConfig	&getConfig(int fd) ;

};