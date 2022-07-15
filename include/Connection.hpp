#pragma once 

#include <iostream>
#include <sys/socket.h>
#include "Request.hpp"
#include "Responce.hpp"
#include "VHost.hpp"
#include "Cgi.hpp"
#include "FileList.hpp"

#define CGI_FILE_IN_PREFIX ".cgi_input"
#define CGI_FILE_OUT_PREFIX ".cgi_output"
#define INPUT_FILE_POST ".inputFile"
class Connection {
	private:
		int					_listennerFd;
		int					_fd;
		VHost*				_vHost;
		Request				_request;
		Responce			_responce;
		routeParams			routeObj;
		location*			currentLoc;
		int					_writed;
		int					_needToWrite;
		std::string 		buffer_in;
		std::string			bodyOut;
		int					bodyRecieved;
		size_t				lastChunkSize; // change to int
		bool				currentChunkNotEnded;
		const std::string	inputBufferName;
		const std::string	cgiOutput;
		std::ofstream		ofs;
		Utils*				utils;

		Connection & operator=(Connection const &other);

	public:
		Connection(Connection const &other);
		Connection(int listenner, int fd, VHost& vHost, Utils* utils);
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
		int			getCurrectCode(void);
		void		setCurrentCode(int fd);
		void		processLocation(void);
		bool		isMoreBody(void);
		void		unchunkBuffer();
		void		preparaBufferForBody();
		void		resetConnection(void);
		void		GET();
		void		POST();
		void		DELETE();
};