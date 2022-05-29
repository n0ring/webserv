#include "Poll.hpp"

#define NO_TIMEOUT -1

void showVector(std::vector<pollfd> v, Server &server, int n) {
	std::vector<pollfd>::iterator it = v.begin();
	std::cout << "current fds in poll:" << v.size() << " nfds=" << n <<  " { ";
	for (; it != v.end() && n != 0; it++, n--) {
		if (it->events == 0 || it->fd == -1) {
			std::cout << RED << "ALERT WE are FUCKED" << std::endl;
		}
		if (server.isFdListener(it->fd)) {
			std::cout << GREEN;
		}
		std::cout << "[ " << it->fd << ": " << (it->events == POLLIN ? "IN" : "OUT") << " ] ";
		std::cout << RESET;
	}
	std::cout << "}" << std::endl;
}

Poll::Poll(Server &server) : _server(server) {
	this->timeout = 1 * 60 * 1000; // 1 min
}

Poll::~Poll(void) {
	std::vector<pollfd>::iterator it = this->fds.begin();
	for (; it != fds.end(); it++) {
		close(it->fd);
	}
};

void	Poll::launch(void) {
	int		curren_size;
	this->_server.setListenersPoll(this->fds);
	this->nfds = this->fds.size();

	while (true) {
		this->nfds = this->fds.size();
		showVector(this->fds, this->_server, this->nfds);
		if (poll(&(this->fds[0]), this->nfds, NO_TIMEOUT) < 0 ) {
			return perror("poll");
		}
		curren_size = this->nfds;
		for (int i = 0; i < curren_size; i++) {
			if (this->fds[i].revents == 0) {
				continue;
			}
			pollfd &pfd = this->fds[i];
			if (pfd.revents != POLLIN  && pfd.revents != POLLOUT) { // connection close
				this->ConnectionPoll.onClientDisconnect(pfd);
				close(pfd.fd);
				this->removeConnection(pfd.fd);
				pfd.fd = -1;
				break ;
			}
			if (this->_server.isFdListener(pfd.fd)) {
				this->ConnectionPoll.onClientConnect(this->_server.getConfig(pfd.fd), this->fds);
			}
			else {
				handleExistConnection(i);
			}
		}
		removeUselessFd();
	}
}
//		 ---  need to  add - remove - change
// poll (fds) 		
// 		 ---

/*
			connection pool
			connection
			

*/

// void	Poll::setNewConnection(pollfd& listenerPollfd) {
// 	int newSocket = 0;
// 	ServerConfig &serverConfig = this->_server.getConfig(listenerPollfd.fd);
// 	do
// 	{
// 		newSocket = serverConfig.acceptNewConnection();
// 		if (newSocket <= 0) {
// 			if (errno != EWOULDBLOCK) {
// 				perror("  accept() failed");
// 				listenerPollfd.revents = 0;
// 				return ;
//             }
// 			break ;
// 		}
// 		this->fds.push_back(make_fd(newSocket, POLLIN));
// 		this->addConnection(newSocket, serverConfig.getListener());
// 		this->nfds++;
// 		// add limit for one loop
// 	} while (newSocket != -1);	
// }	


void	Poll::handleExistConnection(int index) {
	// check for exist connection. if not that means it closed and delete. change fd to -1;
	pollfd&		fdToHandle		= this->fds[index];
	Connection&	connectToHandle	= this->_connections[fdToHandle.fd];
	int ret;
	if (connectToHandle.getFd() == -42) {
		fdToHandle.fd = -1;
		return ;
	}
	if (fdToHandle.revents == POLLIN) {
		ret = connectToHandle.receiveData();
		if (ret == 0) {
			std::cout << "requst received" << std::endl;
			connectToHandle.handleRequest();
			fdToHandle.events = POLLOUT;
		}
		if (ret == -1) {
			fdToHandle.fd = -1;
		}
	}
	if (fdToHandle.revents == POLLOUT ) {
		ret = connectToHandle.sendData();
		if (ret <= 0) {
			fdToHandle.fd = -1;
		}
	}
	fdToHandle.revents = 0;
}


void	Poll::addConnection(int fd, int listener) {
	this->_connections.insert(std::make_pair(fd, Connection(listener, fd)));
}

void Poll::removeConnection(int fd) {
	std::map<int, Connection >::iterator it = this->_connections.begin();
	for (; it != this->_connections.end(); it++) {
		if (it->second.getFd() == fd) {
			std::cout << RED << "remove connection: with fd: " << it->first << RESET << std::endl;
			this->_connections.erase(it);
			break ;
		}
	}
}

void	Poll::removeUselessFd(void) {
	std::sort(this->fds.begin(), this->fds.end(), greaterFd<int>());
	while (this->fds.back().fd == -1) {
		this->fds.pop_back();
		this->nfds--;
	}
}

// pollfd Poll::make_fd(int fd, int event) {
// 	pollfd newfd;
// 	newfd.fd = fd;
// 	newfd.events = event;
// 	newfd.revents = 0;
// 	return newfd;
// }
