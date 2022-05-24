#include "Poll.hpp"

Poll::Poll(Server &server) : _server(server) {
	this->nfds = 1;
	this->timeout = 1 * 60 * 1000; // 1 min
	fds.push_back(make_fd(this->_server.getListener(), POLLIN));
}

Poll::~Poll(void) {
	std::vector<pollfd>::iterator it = this->fds.begin();
	for (; it != fds.end(); it++) {
		close(it->fd);
	}
};

pollfd Poll::make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}

void Poll::delByFd(int fd) {
	close(fd);
	std::map<int, Connection >::iterator it = this->_connections.begin();
	for (; it != this->_connections.end(); it++) {
		if (it->second.getFd() == fd) {
			this->_connections.erase(it);
			break ;
		}
	}
}

template <class T>
struct greaterFd : std::binary_function <T,T,bool> {
  bool operator() (const pollfd& x, const pollfd& y) const {return x.fd > y.fd ;}
};

void	Poll::launch(void) {
	int		rec, curren_size;

	while (true) {
		// std::cout << "start poll" << std::endl;
		rec = poll(&(fds[0]), this->nfds, timeout);
		if (rec < 0) {
			perror("poll");
			break ;
		}
		if (rec == 0) {
			std::cout << "timeout" << std::endl;
			break ;
		}
		curren_size = this->nfds;
		for (int i = 0; i < curren_size; i++) {
			// std::cout << this->fds[i].fd << " : " << this->fds[i].revents << std::endl;
			if (this->fds[i].revents == 0) {
				if (this->_connections[this->fds[i].fd].getReadStatus()) { // read over. handle req
					this->_connections[this->fds[i].fd].handleRequest();
				} 
				continue;
			}
			if (this->fds[i].revents != POLLIN  && this->fds[i].revents != POLLOUT) {
				this->delByFd(this->fds[i].fd);
				this->fds[i].fd = -1;
				continue;
			}
			if (this->fds[i].fd == this->_server.getListener()) {
				setNewConnection();
			}
			else {
				handleExistConnection(i);
			}
		}
		// del -1 fd in stupid way. 
		std::sort(this->fds.begin(), this->fds.end(), greaterFd<int>());
		while (this->fds.back().fd == -1) {
			this->fds.pop_back();
			this->nfds--;
		}
	}
}

void	Poll::setNewConnection() {
	int newSocket;
	do
	{
		newSocket = this->_server.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
				exit(-1);
            }
			break ;
		}
		this->fds.push_back(make_fd(newSocket, POLLIN));
		this->_connections.insert(std::make_pair(newSocket,
			Connection(this->_server.getListener(), newSocket)));
		std::cout << "added new connect: " << newSocket <<  " size now: " << fds.size() << std::endl;
		this->nfds ++;
	} while (newSocket != -1);	
}	

void	Poll::handleExistConnection(int index) {
	pollfd &fdToHandle = this->fds[index];
	Connection &connectToHandle = this->_connections[fdToHandle.fd];

	if (fdToHandle.revents == POLLIN) {
		if ( connectToHandle.getReadStatus() == false) {
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
