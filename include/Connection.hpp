#pragma once 

#include <iostream>
#include <sys/socket.h>
#include "Request.hpp"
#include "Responce.hpp"
#include "VHost.hpp"
#include "Cgi.hpp"

#define CGI_FILE_IN_PREFIX ".cgi_input"
#define CGI_FILE_OUT_PREFIX ".cgi_output"
#define DEFAULT_ERROR_PAGE_PREFIX ".defaultErrorPage"
#define INPUT_FILE_POST ".inputFile"
class Connection {
	private:
		int			_listennerFd;
		int			_fd;
		VHost*		_vHost;
		Request		_request;
		Responce	_responce;
		routeParams	routeObj;
		location*	currentLoc;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		int			bodyRecieved;
		size_t		lastChunkSize;
		bool		currentChunkNotEnded;
		std::string inputBufferName;

		std::string		defaultErrorPageName;
		std::string		cgiOutput;
		std::ofstream	ofs;



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
		void		setResponceFile();
		void		handleRequest();
		void		checkForVhostChange();
		void		executeOrder66();
		void		saveBody();
		std::string getErrorPageName(int code);
		int			getCurrectCode(void) { return this->_request.getCurrentCode(); }
		void		setCurrentCode(int fd) { this->_request.setCurrentCode(fd);}
		void		processLocation(void);
		bool		isMoreBody(void);
		void		POST();
		void		unchunkBuffer();
		void		preparaBufferForBody();
};