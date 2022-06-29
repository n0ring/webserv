#pragma once 

#include <iostream>
#include <sys/socket.h>
#include "Request.hpp"
#include "Responce.hpp"
#include "VHost.hpp"
#include "Cgi.hpp"
class Connection {
	private:
		int			_listennerFd;
		int			_fd;
		VHost*		_vHost;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		std::string body; // use? 
		Request		_request;
		Responce	_responce;
		routeParams	routeObj;
		location*	currentLoc;
		std::string cgiIput;
		int 		cgiIputFd;
		std::string cgiOutput;
		std::string	defaultErrorPageName;

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
		void		sendBodyToFile();
		std::string getErrorPageName(int code);
		void		setCgiInputFd(int fd) {this->cgiIputFd = fd;}
		int			getCurrectCode(void) { return this->_request.getCurrentCode(); }
		void		setCurrentCode(int fd) { this->_request.setCurrentCode(fd);}
		std::string& getCgiInputFileName(void) { return this->cgiIput; }
		void		processLocation(void);
};