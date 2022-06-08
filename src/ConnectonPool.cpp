#include "Cp.hpp"

ConnectionPool::ConnectionPool() {}

ConnectionPool::~ConnectionPool(void) {
	this->_pool.clear();
}

void	ConnectionPool::onClientConnect(VHost& serverConfig, std::vector<pollfd>& fds,
			std::vector<pollfd>::iterator& iter) {
	int newSocket = 0;
	int pos = std::distance(fds.begin(), iter);

	while (newSocket != -1) {
		newSocket = serverConfig.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
			}
			break ;
		}
		fds.push_back(make_fd(newSocket, POLLIN));
		this->_pool.insert(std::make_pair(newSocket, Connection(serverConfig.getListener(), newSocket)));
		std::cout << "connect added: " << this->_pool.size() << std::endl;
	}
	iter = fds.begin() + pos; // need to update iter because realloc in vector 
}

void	ConnectionPool::onClientDisconnect(std::vector<pollfd>::iterator& iter,
			std::vector<pollfd> &fds) { // take iterator &
	close(iter->fd);
	this->_pool.erase(iter->fd);
	iter = fds.erase(iter);
}

void	ConnectionPool::onClientDataExchange(std::vector<pollfd>::iterator& iter,
												VHost& viHost) {
	int ret;
	if (this->_pool.count(iter->fd) == 0 && viHost.getListener() == -1) {
		std::cerr << "This should never happend" << std::endl;
		// do something here for delete connection 
	}
	// end of receive 
	// receive continue
	if (iter->revents == POLLIN) {
		// handle 
		ret = this->_pool[iter->fd].receiveData();
		if (ret < 0) {
			// delete from pool? 
		}
		if (ret == 0) { // end of transfer
			// prepare responce.
			viHost.handleRequest(this->_pool[iter->fd].getRequestObj(),
				this->_pool[iter->fd].getResponceObj());
			this->_pool[iter->fd].prepareResponceToSend();
			iter->events = POLLOUT;
		}
	}
	if (iter->revents == POLLOUT) {
		ret = this->_pool[iter->fd].sendData();
		if (ret == 0) {
			iter->events = POLLIN;
		}
		if (ret == -1) {
			// delete from pool?
		}
	}
	iter->revents = 0;
}

int ConnectionPool::getConnectionListener(int fd) {
	if (this->_pool.count(fd) == 0) {
		return -1;
	}
	return this->_pool[fd].getListener();
}
