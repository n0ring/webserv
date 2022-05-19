#include "Server.hpp"

Server::Server(void) : _ip("127.0.0.1") {
	int rec;
	_port = 8080;
	_backlog = 10;
	
	this->setupSocket();
	this->setupSockAddr_in();
	
	rec = bind(_listener, &_address, this->_addrlen);
	if (rec == -1) {
		error("bind");
		exit(-1);
	}
	rec = listen(_listener, _backlog);
	if (rec == -1) {
		error("listen");
		exit(-1);
	}
}

Server::~Server(void) {
	close(_listener);
}

void Server::start() {
	// selectLogic();
	pollLogic();
}

void	Server::setupSocket(void) {
	_listener = socket(PF_INET, SOCK_STREAM, 0);
	if (_listener == -1) {
		perror("socket");
		exit(-1);
	}
	fcntl(_listener, F_SETFL, O_NONBLOCK);
	int yes=1;
	setsockopt(_listener, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
}

void	Server::setupSockAddr_in(void) {
	sockaddr_in *address;
	
	this->_addrlen = (socklen_t) sizeof(this->_address);
	// bzero(&(this->_address), sizeof(this->_address));

	address = reinterpret_cast<sockaddr_in *>(&this->_address);
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = inet_addr(_ip.c_str());
	// address->sin_addr.s_addr = htons(INADDR_ANY);
	address->sin_port = htons(_port);
}

void	Server::error(std::string const &s) {
	perror(s.c_str());
	close(this->_listener);
}

void Server::sendBuf(int fd, const char *buf, int len) {
	int rev = 0;
	int sended = 0;
	while (sended < len) {
		rev = send(fd, buf + sended, len - sended, 0);
		if (rev == -1) {
			this->error("send");
			return ;
		}
		sended += rev;
	}
}

/*************************************************/
/***************** SELECT OPTION *****************/
/*************************************************/

void Server::selectLogic() {
	int		fdmax = this->_listener;
	fd_set	tmp_set, masterSet, recipientSet;
	
	FD_ZERO(&masterSet);
	FD_ZERO(&recipientSet);
	FD_SET(this->_listener, &masterSet);
	while (true) {
		tmp_set = masterSet;
		select(fdmax + 1, &tmp_set, &recipientSet, NULL, NULL);
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_set)) {
				if (i == this->_listener) {
					this->select_SetNewConnection(fdmax, &masterSet, &recipientSet);
				}
				else { 
					this->select_HandleExistConnection(i, fdmax, &masterSet, &recipientSet);
				}
			}
		}
	}
	FD_ZERO(&masterSet);
	FD_ZERO(&recipientSet);
}

void	Server::select_SetNewConnection(int &fdmax, fd_set *masterSet, fd_set *recipientSet) {
	int		newSocket;

	newSocket = accept(this->_listener, &this->_address, &this->_addrlen);
	if (newSocket == -1) {
		this->error("accept");
		return ;
	}
	FD_SET(newSocket, masterSet);
	FD_SET(newSocket, recipientSet);
	if (newSocket > fdmax) { fdmax = newSocket; }
	std::cout << "New connect on fd " << newSocket << std::endl;
}

void	Server::select_HandleExistConnection(int fdToHandle, int &fdmax, fd_set *masterSet, fd_set *recipientSet) {
	char buf[1024];
	int rec = recv(fdToHandle, buf, 1024, 0);
	if (rec == 0) {
		close(fdToHandle);
		FD_CLR(fdToHandle, masterSet);
		FD_CLR(fdToHandle, recipientSet);
		std::cout << RED << fdToHandle << " was removed" << RESET << std::endl;
	} else {
		std::cout << GREEN << buf << std::endl << RESET;
		for (int k = 0; k <= fdmax; k++) {
			if (FD_ISSET(k, recipientSet)) {
				if (k != this->_listener) {
					std::string responce ("message from ");
					responce.append(std::to_string(fdToHandle));
					responce.append(": ");
					responce.append(buf);
					sendBuf(k, responce.c_str(), responce.length());
				}
			}
		}
	}
	bzero(buf, 1024);
}
/*************************************************/
/************* END OF SELECT OPTION **************/
/*************************************************/


/*************************************************/
/****************** POLL OPTION ******************/
/*************************************************/



void sendToAll(int nfds, pollfd *fds, char *buf, int author) {
	for (int i = 0; i < nfds - 1; i++) {
		std::cout << "Check " << i << " " << fds[i].fd << std::endl;
		std::string responce ("message from ");
		responce.append(std::to_string(author));
		responce.append(": ");
		responce.append(buf);
		responce.append("\n");
		send(fds[i].fd, responce.c_str(), responce.length(), 0);
	}
}

void	Server::poll_HandleExistConnection(int fdToHandle, pollfd *fds, int &nfds, pollfd *outfds) {
	char buf[1024];
	bzero(buf, 1024);
	if (fds[fdToHandle].revents == POLLIN) {
		int rec = recv(fds[fdToHandle].fd, buf, 1024, 0);
		if (rec == -1) {
			error("recv");
			return ;
		}
		if (rec == 0) {
			// del from list fds
		}
		std::cout << GREEN << buf << RESET << std::endl;
		sendToAll(nfds, outfds, buf, fds[fdToHandle].fd);
	}
}


void	Server::pollLogic(void) {
	pollfd fds[200], outfds[200]; 
	int		nfds = 1, rec, currentSize;
	int timeout = 3 * 60 * 1000; // 3 min

	bzero(fds, sizeof(fds));
	bzero(outfds, sizeof(outfds));
	fds[0].fd = this->_listener;
	fds[0].events = POLLIN;

	while (true) {
		rec = poll(fds, nfds, timeout);
		if (rec < 0) {
			error("poll");
			break ;
		}
		currentSize = nfds;
		for (int i = 0; i < currentSize; i++) {
			// std::cout << "check fd #" << i << " " << fds[i].fd << std::endl; 
			if (fds[i].revents == 0) continue;
			if (fds[i].revents != POLLIN && fds[i].revents != POLLOUT) {
				std::cerr << "Not expeted revent with " << fds[i].fd << "->" << fds[i].revents << std::endl;
				return ;
			}
			if (fds[i].fd == this->_listener) {
				poll_SetNewConnection(fds, nfds, outfds);
			}
			else {
				poll_HandleExistConnection(i, fds, nfds, outfds);
			}
		}
	}
}

void	Server::poll_SetNewConnection(pollfd *fds, int &nfds, pollfd *outfds) {
	int newSocket;
	do
	{
		newSocket = accept(this->_listener, &this->_address, &this->_addrlen);
		if (newSocket < 0) {
			if (errno != EWOULDBLOCK) {
              perror("  accept() failed");
			  exit(-1);
            }
			break ;
		}
		fds[nfds].fd = newSocket;
		fds[nfds].events = POLLIN;
		outfds[nfds - 1].fd = newSocket;
		outfds[nfds - 1].events = POLLOUT;
		nfds++;
	} while (newSocket != -1);
	
}

