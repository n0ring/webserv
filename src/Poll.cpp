#include "Poll.hpp"

void showVector(std::vector<pollfd> v, Server &server) {
	std::vector<pollfd>::iterator it = v.begin();
	std::cout << "current fds in poll: { ";
	for (; it != v.end(); it++) {
		if (server.isFdListener(it->fd)) {
			std::cout << GREEN;
		}
		std::cout << "[ " << it->fd << ": " << (it->events == POLLIN ? "IN" : "OUT") << " ] ";
		std::cout << RESET;
	}
	std::cout << "}" << std::endl;
}

Poll::Poll(Server &server) : _server(server) {
	this->nfds = 1;
	this->timeout = 1 * 60 * 1000; // 1 min
}

Poll::~Poll(void) {
	std::vector<pollfd>::iterator it = this->fds.begin();
	for (; it != fds.end(); it++) {
		close(it->fd);
	}
};

void	Poll::launch(void) {
	int		rec, curren_size;
	this->_server.setListenersPoll(this->fds);

	while (true) {
		showVector(this->fds, this->_server);
		rec = poll(&(this->fds[0]), this->nfds, -1);
		if (rec < 0) {
			perror("poll");
			break ;
		}
		curren_size = this->nfds;
		for (int i = 0; i < curren_size; i++) {
			if (this->fds[i].revents == 0) {
				if (this->_connections[this->fds[i].fd].isReadStarted()) { // read over. handle req
					this->_connections[this->fds[i].fd].handleRequest();
				}
				continue;
			}
			if (this->fds[i].revents != POLLIN  && this->fds[i].revents != POLLOUT) {
				this->removeConnection(this->fds[i].fd);
				this->fds[i].fd = -1;
				continue;
			}
			if (this->_server.isFdListener(this->fds[i].fd)) {
				setNewConnection(this->fds[i].fd);
			}
			else {
				handleExistConnection(i);
			}
		}
		removeUselessFd();
	}
}

void	Poll::setNewConnection(int listener) {
	int newSocket;
	ServerConfig &serverConfig = this->_server.getConfig(listener);
	do
	{
		newSocket = serverConfig.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
				exit(-1);
            }
			break ;
		}
		this->fds.push_back(make_fd(newSocket, POLLIN));
		this->addConnection(newSocket, serverConfig.getListener());

		this->nfds++;
	} while (newSocket != -1);	
}	

void	Poll::handleExistConnection(int index) {
	pollfd &fdToHandle = this->fds[index];
	Connection &connectToHandle = this->_connections[fdToHandle.fd];

	if (fdToHandle.revents == POLLIN) {
		if ( connectToHandle.isReadStarted() == false) {
			this->fds.push_back(make_fd(fdToHandle.fd, POLLOUT));
			this->nfds++;
		}
		connectToHandle.receiveData();
	}
	if (fdToHandle.revents == POLLOUT && connectToHandle.getHandleStatus()) {
		int ret = connectToHandle.sendData();
		if (ret == 0) { // end of send
			fdToHandle.fd = -1; // need to del
		}
	}
	fdToHandle.revents = 0;
}


void	Poll::addConnection(int fd, int listener) {
	this->_connections.insert(std::make_pair(fd, Connection(listener, fd)));
}

void Poll::removeConnection(int fd) {
	close(fd);
	std::map<int, Connection >::iterator it = this->_connections.begin();
	for (; it != this->_connections.end(); it++) {
		if (it->second.getFd() == fd) {
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

pollfd Poll::make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}


