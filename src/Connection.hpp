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

//		request (server, text)
//		resoince (server, text)

	public:
		Connection(int listenner, int fd);
		Connection();
		Connection(Connection const &other);
		Connection & operator=(Connection const &other);
		~Connection(void);

		int		receiveData();
		void	handleRequest(); // func to server?? 
		int		sendData();


		int		getFd() const;
		bool	isReadStarted() const;
		bool	getHandleStatus() const;
};