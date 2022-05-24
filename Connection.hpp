#include <iostream>
#include <sys/socket.h>

class Connection {
	private:
		int			_listennerFd;
		int			_fd;
		// int  		_readed;
		int			_writed;
		int			_needToWrite;
		std::string buffer_in;
		std::string buffer_out;
		bool		_isStartRead;
		bool		_isDataHandled;
		char		*_bufToSend;

	public:
		Connection(int listenner, int fd);
		Connection();
		Connection(Connection const &other);
		Connection & operator=(Connection const &other);
		~Connection(void);
		int getFd() const;
		int receiveData();
		void handleRequest(); // func to server?? 
		int sendData();
		bool	getReadStatus() const;
		bool	getHandleStatus() const;
		void	resetReadStatus();
		void	resetHandleStatus();

		// proccessing (POLLIN, POLLOUT);
};