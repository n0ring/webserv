#include "Cp.hpp"

ConnectionPool::ConnectionPool() {}

ConnectionPool::~ConnectionPool(void) {
	this->_pool.clear();
}

void	ConnectionPool::onClientConnect(VHost& vHost, std::vector<pollfd>& fds,
			std::vector<pollfd>::iterator& iter) {
	int newSocket = 0;
	int pos = std::distance(fds.begin(), iter);

	while (newSocket != -1) {
		newSocket = vHost.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
			}
			break ;
		}
		fds.push_back(make_fd(newSocket, POLLIN));
		this->_pool.insert(std::make_pair(newSocket, Connection( vHost.getListener(), newSocket, vHost )));
		std::cout << "connect added: " << this->_pool.size() << std::endl;
	}
	iter = fds.begin() + pos; // need to update iter because realloc in vector 
}

void	ConnectionPool::onClientDisconnect(std::vector<pollfd>::iterator& iter,
			std::vector<pollfd> &fds) { // take iterator &make 
	close(iter->fd);
	this->_pool.erase(iter->fd);
	iter = fds.erase(iter);
}

int		ConnectionPool::onClientDataExchange(std::vector<pollfd>::iterator& iter) {
	int ret;
	std::map<int, Connection>::iterator it_connection;
	it_connection = this->_pool.find(iter->fd);
	if (it_connection == this->_pool.end()) {
		std::cout << "connetion not found for some reasons..." << std::endl;
		return -1;
	}
	if (iter->revents == POLLIN) {
		// handle 
		ret = it_connection->second.receiveData();
		if (ret < 0) {
			std::cout << "receive data error\n";
			return -1;
		}
		if (ret == 0) {
			it_connection->second.prepareResponceToSend();
			iter->events = POLLOUT;
		}
	}
	else if (iter->revents == POLLOUT) {
		ret = it_connection->second.sendData();
		if (ret == 0) {
			iter->events = POLLIN;
			std::cout << "send is over\n";
		}
		if (ret == -1) {
			std::cout << "send error is over\n";
			return -1;
		}
	}
	iter->revents = 0;
	return 0;
}