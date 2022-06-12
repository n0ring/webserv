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
		VHost&		_vHost;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		Request		_request;
		Responce	_responce;

		// fileToSave. 

	public:
		Connection(int listenner, int fd, VHost& vHost);
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
		VHost&		getVhost(void) { return this->_vHost; }
		void		setResponce();
		void		handleRequest();
};