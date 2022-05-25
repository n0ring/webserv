#include "Server.hpp"

Server::Server(void)  { }

Server::~Server(void) { }


/*
1. create serverConfig objects while parsing 
2. in loop setup them (socket, addrset, listen bind)
*/
void Server::start() {
	Poll pollObj(*this);
	// 1 loop? 
	this->_configs.push_back(ServerConfig(8080, "127.0.0.1", 10));
	this->_configs.push_back(ServerConfig(8081, "127.0.0.1", 10));
	this->_configs.push_back(ServerConfig(8082, "127.0.0.1", 10));

	// 2. setupServers in loop
	for (int i = 0; i < (int) this->_configs.size(); i++) {
		this->_listeners.push_back(this->_configs[i].setup());
	}

	pollObj.launch();

}

bool 	Server::isFdListener(int fd) {
	std::vector<int>::iterator res = std::find(this->_listeners.begin(),
		this->_listeners.end(), fd);
	if (res == this->_listeners.end()) {
		return false;
	}
	return true;
}

pollfd make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;

	return newfd;
}

void Server::setListenersPoll(std::vector<pollfd> &v) {
	std::vector<ServerConfig>::iterator it = this->_configs.begin();
	std::vector<ServerConfig>::iterator ite = this->_configs.end();
	for (; it != ite; it++) {
		v.push_back(make_fd(it->getListener(), POLLIN));
	}
}

ServerConfig & Server::getConfig(int fd) { // if not found we sooooo fucked
	std::vector<ServerConfig>::iterator it = this->_configs.begin();
	std::vector<ServerConfig>::iterator ite = this->_configs.end();
	for (; it != ite; it++) {
		if (it->getListener() == fd) {
			break ;
		}
	}
	return *it;
}
