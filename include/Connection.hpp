#pragma once 

#include <iostream>
#include <sys/socket.h>
#include "Request.hpp"
#include "Responce.hpp"
#include "VHost.hpp"

class Connection {
	private:
		int			_listennerFd;
		int			_fd;
		VHost*		_vHost;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		std::string body;
		Request		_request;
		Responce	_responce;
		routeParams	routeObj;
		location*	currentLoc;
		std::string cgiIput;
		int 		cgiIputFd;
		std::string cgiOutput;


		// fileToSave. open file? 

	public:
		Connection(Connection const &other);
		Connection(int listenner, int fd, VHost& vHost);
		Connection & operator=(Connection const &other);
		~Connection(void);

		int		receiveData();
		void	prepareResponceToSend(); // func to server?? 
		int		sendData();

		int			getFd() const;
		int			getListener(void) const;
		Request&	getRequestObj();
		Responce&	getResponceObj();
		void		setResponce();
		void		handleRequest();
		void		checkForVhostChange();
		void		executeOrder66();

};