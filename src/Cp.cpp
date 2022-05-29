#include "Cp.hpp"

Cp::Cp() {}

Cp::~Cp(void) {
	this->_pool.clear();
}

void	Cp::onClientConnect(ServerConfig& serverConfig, std::vector<pollfd>& fds) {
	int newSocket = 0;

	while (newSocket != -1) {
		newSocket = serverConfig.acceptNewConnection();
		if (newSocket <= 0) {
			if (errno != EWOULDBLOCK) {
				perror("  accept() failed");
				return ;
            }
			break ;
		}
		fds.push_back(make_fd(newSocket, POLLIN));
		this->_pool.insert(std::make_pair(newSocket, Connection(serverConfig.getListener(), newSocket)));
		std::cout << "connect added: " << this->_pool.size() << std::endl;
	}	
}

void	Cp::onClientDisconnect(pollfd& pfd) {
	close(pfd.fd);
	
}

