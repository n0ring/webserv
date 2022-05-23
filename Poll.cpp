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


void delNumber(std::vector<pollfd> &v, int i) {
	std::vector<pollfd>::iterator it = v.begin();
	while (it != v.end() && i) {
		it++;
		i--;
	}
	v.erase(it);
}

pollfd Poll::make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}

void Poll::deleteFd(int index) {
	std::vector<pollfd>::iterator it = this->fds.begin() + index;
	if (it != this->fds.end()) {
		close(it->fd);
		this->fds.erase(it);
	}
	this->nfds--;
}

void	Poll::launch(void) {
	int		rec, curren_size;

	while (true) {
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
			if (this->fds[i].revents == 0) continue;
			if (this->fds[i].revents != POLLIN) {
				close(this->fds[i].fd);
				this->fds.erase(this->fds.begin() + i);
				break; // change this. 
			}
			if (this->fds[i].fd == this->_server.getListener()) {
				setNewConnection();
				this->fds[i].revents = 0;
			}
			else {
				handleExistConnection(this->fds[i]);
				this->fds[i].revents = 0;
			}
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
		std::cout << "added new connect: " << newSocket <<  " " << fds.size() << std::endl;
		nfds++;
	} while (newSocket != -1);	
}

void	Poll::handleExistConnection(pollfd &fdToHandle) {
	char buf[1024];
	bzero(buf, 1024);
	if (fdToHandle.revents == POLLIN) {
		int rec = recv(fdToHandle.fd, buf, 1024, 0);
		if (rec == -1) {
			perror("recv");
			return ;
		}
		if (rec == 0) {
			// remove fd. connection close;
		}
		std::cout << GREEN << buf << RESET << std::endl;
		send(fdToHandle.fd, buf, 1024, 0);
	}
}
