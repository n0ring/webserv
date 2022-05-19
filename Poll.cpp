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
	for (it = outfds.begin(); it != outfds.end(); it++) {
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

void Poll::sendToAll(char *buf, int author) {
	std::vector<pollfd>::iterator it = outfds.begin();
	std::string responce ("message from ");

	responce.append(std::to_string(author));
	responce.append(": ");
	responce.append(buf);
	responce.append("\n");
	for (; it != outfds.end(); it++) {
		send(it->fd, responce.c_str(), responce.length(), 0);
	}
}



pollfd Poll::make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}

void Poll::deleteFd(int fdToDel) {
	std::vector<pollfd>::iterator it = this->outfds.begin();
	for (; it != this->outfds.end(); it++) {
		if (it->fd == fdToDel) {
			break;
		}
	}
	if (it != this->outfds.end()) {
		this->outfds.erase(it);
	}
	it = this->fds.begin();
	for (; it != this->fds.end(); it++) {
		if (it->fd == fdToDel) {
			break;
		}
	}
	if (it != this->fds.end()) {
		this->fds.erase(it);
	}
	close(fdToDel);
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
				this->deleteFd(this->fds[i].fd);
				break ;
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
		this->outfds.push_back(make_fd(newSocket, POLLOUT));
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
			this->deleteFd(fdToHandle.fd);
		}
		std::cout << GREEN << buf << RESET << std::endl;
		sendToAll(buf, fdToHandle.fd);
	}
}
