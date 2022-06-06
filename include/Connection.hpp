#pragma once 

#include <iostream>
#include <sys/socket.h>
#include "Request.hpp"
#include "Responce.hpp"


class Connection {
	private:
		int			_listennerFd;
		int			_fd;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		Request		_request;
		Responce	_responce;

		// requestObj
		// responceObj


	public:
		Connection(int listenner, int fd);
		Connection();
		Connection(Connection const &other);
		Connection & operator=(Connection const &other);
		~Connection(void);

		int		receiveData();
		void	prepareResponceToSend(); // func to server?? 
		int		sendData();

		int			getFd() const;
		int			getListener(void) const;
		Request&	getRequestObj();
		Responce&	getResponceObj();
};