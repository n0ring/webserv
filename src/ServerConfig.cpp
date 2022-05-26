#include "ServerConfig.hpp"

ServerConfig::ServerConfig(void) {}
ServerConfig::~ServerConfig(void) {
	close(this->_listener);
}

ServerConfig::ServerConfig(int port, std::string ip, int bl) : _port(port),
		_ip(ip), _backlog(bl) {

}


ServerConfig::ServerConfig(ServerConfig const &other) : _port(other._port),
	_ip(other._ip), _backlog(other._backlog) {

}

ServerConfig & ServerConfig::operator=(ServerConfig const &other) {
	if (this != &other) {
		this->_listener = other._listener;
		this->_address = other._address;
		this->_addrlen = other._addrlen;
		this->_port = other._port;
		this->_ip = other._ip;
		this->_backlog = other._backlog;
	}
	return *this;
}

int ServerConfig::setup(void) {
	this->setupSocket();
	this->setupSockAddr_in();

	int	rec = bind(this->_listener, &_address, this->_addrlen);
	if (rec == -1) {
		perror("bind");
		return (-1);
	}
	rec = listen(_listener, _backlog);
	if (rec == -1) {
		perror("listen");
		return (-1);
	}

	return this->_listener;
}


void	ServerConfig::setupSocket(void) {
	this->_listener = socket(PF_INET, SOCK_STREAM, 0); // maybe add second socket for udp
	if (this->_listener == -1) {
		perror("socket");
		exit(-1);
	}
	fcntl(_listener, F_SETFL, O_NONBLOCK);
	int yes=1;
	setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
}

void	ServerConfig::setupSockAddr_in(void) {
	sockaddr_in *address;
	
	bzero(&(this->_address), sizeof(this->_address));
	this->_addrlen = (socklen_t) sizeof(this->_address);

	address = reinterpret_cast<sockaddr_in *>(&this->_address);
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = inet_addr(this->_ip.c_str());
	// address->sin_addr.s_addr = htons(INADDR_ANY);
	address->sin_port = htons(this->_port);
}


int ServerConfig::getListener(void) const {
	return this->_listener;
}

int		ServerConfig::acceptNewConnection() {
	int newSocket =  accept(this->_listener, &this->_address, &this->_addrlen);
	if (newSocket < 0) {
		return -1;
	}
	std::cout << "accepting connect on fd " << this->_listener << std::endl;
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	return newSocket;
}