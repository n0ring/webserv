#include "Poll.hpp"

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

/*
poll after ending recv get event on copy fd with event POLLOUT (its always ready),
but in loop we check first fd with event IN.
so if status reading that read if over.can handle now
*/

void	Poll::launch(void) {
	int		curren_size;
	this->_server.setListenersPoll(this->fds);
	this->nfds = this->fds.size();
	if (this->nfds == 0) {
		std::cerr << "No active servers." << std::endl;
		return ;
	}
	while (true) {
		if (poll(&(this->fds[0]), this->nfds, -1) < 0 ) {
			return perror("poll");
		}
		curren_size = this->nfds;
		for (int i = 0; i < curren_size; i++) {
			if (this->fds[i].revents == 0) {
				if (this->_connections[this->fds[i].fd].isReading()) { // read over. handle req
					this->_connections[this->fds[i].fd].handleRequest();
				}
				continue;
			}
			if (this->fds[i].revents != POLLIN  && this->fds[i].revents != POLLOUT) { // connection close 
				close(this->fds[i].fd);
				this->removeConnection(this->fds[i].fd);
				this->fds[i].fd = -1;
				break ;
			}
			if (this->_server.isFdListener(this->fds[i].fd)) {
				setNewConnection(this->fds[i]);
			}
			else {
				handleExistConnection(i);
			}
		}
		removeUselessFd();
	}
}

void	Poll::setNewConnection(pollfd& listenerPollfd) {
	int newSocket = 0;
	ServerConfig &serverConfig = this->_server.getConfig(listenerPollfd.fd);
	do
	{
		newSocket = serverConfig.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
				listenerPollfd.revents = 0;
				return ;
            }
			break ;
		}
		this->fds.push_back(make_fd(newSocket, POLLIN));
		this->addConnection(newSocket, serverConfig.getListener());
		this->nfds++;
		// showVector(this->fds, this->_server, this->nfds);
	} while (newSocket != -1);	
}	


/*
recieve or send
*/
void	Poll::handleExistConnection(int index) {
	// check for exist connection. if not that means it closed and delete. change fd to -1;
	pollfd&		fdToHandle		= this->fds[index];
	Connection&	connectToHandle	= this->_connections[fdToHandle.fd];

	if (connectToHandle.getFd() == -42) {
		fdToHandle.fd = -1;
		return ;
	}
	if (fdToHandle.revents == POLLIN) {
		if ( connectToHandle.isReading() == false ) { // no good
			this->fds.push_back(make_fd(fdToHandle.fd, POLLOUT));
			this->nfds++;
			// showVector(this->fds, this->_server, this->nfds);
		}
		if (connectToHandle.receiveData() <= 0 ) {
			this->fds.back().fd = -1;
		}
	} else if (fdToHandle.revents == POLLOUT && connectToHandle.isReading() == false) {
		if (connectToHandle.sendData() <= 0) {
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

pollfd Poll::make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}


