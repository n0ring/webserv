#include "Cp.hpp"

Cp::Cp() {}

Cp::~Cp(void) {
	this->_pool.clear();
}

void	Cp::onClientConnect(ServerConfig& serverConfig, std::vector<pollfd>& fds,
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

void	Cp::onClientDisconnect(std::vector<pollfd>::iterator& iter, std::vector<pollfd> &fds) { // take iterator &
	close(iter->fd);
	this->_pool.erase(iter->fd);
	iter = fds.erase(iter);
}

void	Cp::onClientDataExchange(std::vector<pollfd>::iterator& iter, ServerConfig& viHost) {
	int ret;
	(void) viHost;
	if (iter->revents == POLLIN) {
		ret = this->_pool[iter->fd].receiveData();
		if (ret == 0) {
			std::cout << "requst received" << std::endl;
			iter->events = POLLOUT;
		}
		if (ret < 0) {
			// delete from pool? 
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