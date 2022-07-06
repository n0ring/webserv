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

	parser.parseConfig(this->_vHosts, configName);
	std::cout << "Servers: " << std::endl;
	for (int i = 0; i < (int) this->_vHosts.size(); i++) {
		this->_vHosts[i].toString();
		this->fds.push_back(make_fd(this->_vHosts[i].setup(), POLLIN));
	}
}


void	Server::start(std::string configName) {
	std::vector<pollfd>::iterator it;
	bool							readyForWork = false;
	this->setupServers(configName);
	if (this->_vHosts.size() > 0) {
		readyForWork = true;
	}
	else {
		std::cerr << "no vHosts" << std::endl;
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
				this->connectionPool.onClientDisconnect(it, this->fds);
				continue;
			}
			if (it->revents == POLLOUT || !this->isFdListener(it->fd)) {
				if (this->connectionPool.onClientDataExchange(it) == -1) {
					this->connectionPool.onClientDisconnect(it, this->fds);
					continue;
				}
			} else {
				this->connectionPool.onClientConnect(this->getVHost(it->fd), this->fds, it);
			}
			it++;
		}
	}
}

bool 	Server::isFdListener(int fd) {
	std::vector<VHost>::iterator it = this->_vHosts.begin();
	std::vector<VHost>::iterator ite = this->_vHosts.end();
	while (it != ite) {
		if (fd == it->getListener()) {
			return true;
		}
		it++;		
	}
	return false;
}


VHost & Server::getVHost(int fd) { // if not found we sooooo fucked
	std::vector<VHost>::iterator it = this->_vHosts.begin();
	std::vector<VHost>::iterator ite = this->_vHosts.end();
	for (; it != ite; it++) {
		if (it->getListener() == fd) {
			break ;
		}
	}
	return *it;
}
