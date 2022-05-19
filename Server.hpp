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


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

class Server {
	private: 
		int			_port;
		int			_listener;
		sockaddr	_address;
		socklen_t	_addrlen;
		std::string	_ip;
		int 		_backlog;


		// utils
		void	sendBuf(int fd, const char *buf, int len);
		void	error(std::string const &s);
		void	setupSockAddr_in(void);
		void	setupSocket(void);

		void	selectLogic(void);
		void	select_SetNewConnection(int &fdmax, fd_set *masterSet, fd_set *recipientSet);
		void	select_HandleExistConnection(int fdToHandle, int &fdmax, fd_set *masterSet, fd_set *recipientSet);

		void	pollLogic(void);
		void	poll_SetNewConnection(pollfd *readFds, int &nfds, pollfd *outfds);
		void	poll_HandleExistConnection(int fdToHandle, pollfd *fds, int &nfdsx, pollfd *outfds);

	public: 
		Server(void);
		~Server(void);

		void start(void);
		



};