#include "Server.hpp"

Server::Server(void)  { }

Server::~Server(void) { }


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

/*
1. create serverConfig objects while parsing 
2. in loop setup them (socket, addrset, listen bind)
*/
void Server::setupServers(std::string configName) {
	Parser parser;

	parser.parseConfig(this->_configs, configName);
	// this->_configs.push_back(ServerConfig(8080, "127.0.0.1", 10));
	
	for (int i = 0; i < (int) this->_configs.size(); i++) {
		this->fds.push_back(make_fd(this->_configs[i].setup(), POLLIN));
	}
}


void	Server::start(std::string configName) {
	std::vector<pollfd>::iterator it;
	bool							readyForWork = false;

	this->setupServers(configName);
	if (this->_configs.size() > 0) {
		readyForWork = true;
	}
	while (readyForWork) { //////////// !!!!!!!!
		this->nfds = this->fds.size();
		showVector(this->fds, *this, this->nfds);
		if (poll(&(this->fds[0]), this->nfds, NO_TIMEOUT) < 0 ) {
			return perror("poll");
		}
		it = this->fds.begin();
		while (it != this->fds.end()) {
			if (it->revents == 0) {
				it++;
				continue;
			}
			else if (it->revents != POLLIN && it->revents != POLLOUT) {
				this->ConnectionPool.onClientDisconnect(it, this->fds);
				continue;
			}
			if (this->isFdListener(it->fd)) {
				this->ConnectionPool.onClientConnect(this->getConfig(it->fd), this->fds, it);
			}
			else {
				this->ConnectionPool.onClientDataExchange(it);
			}
			it++;
		}
	}
}

bool 	Server::isFdListener(int fd) {
	std::vector<ServerConfig>::iterator it = this->_configs.begin();
	std::vector<ServerConfig>::iterator ite = this->_configs.end();
	while (it != ite) {
		if (fd == it->getListener()) {
			return true;
		}
		it++;		
	}
	return false;
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
