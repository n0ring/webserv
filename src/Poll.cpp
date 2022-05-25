#include "Poll.hpp"

void showVector(std::vector<pollfd> v) {
	std::vector<pollfd>::iterator it = v.begin();
	std::cout << "{ ";
	for (; it != v.end(); it++) {
		std::cout << "[ " << it->fd << " : " << (it->events == POLLIN ? "IN" : "OUT") << " ] ";
	}
	std::cout << "}" << std::endl;
}

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

template <class T>
struct greaterFd : std::binary_function <T,T,bool> {
  bool operator() (const pollfd& x, const pollfd& y) const {return x.fd > y.fd ;}
};

void	Poll::launch(void) {
	int		rec, curren_size;
	showVector(this->fds);


	while (true) {
		rec = poll(&(this->fds[0]), this->nfds, timeout);
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
			if (this->fds[i].fd == this->_server.getListener()) {
				setNewConnection();
			}
			else {
				handleExistConnection(i);
			}
		}
		removeUselessFd();
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
		this->addConnection(newSocket, this->_server.getListener());
		showVector(this->fds);
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
			showVector(this->fds);
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

void	Poll::removeUselessFd(void) {
	std::sort(this->fds.begin(), this->fds.end(), greaterFd<int>());
	while (this->fds.back().fd == -1) {
		this->fds.pop_back();
		this->nfds--;
	}
}