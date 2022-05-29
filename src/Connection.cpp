#include "Connection.hpp"

#define BUFFER 2048

Connection::Connection(void) : _listennerFd(-42), _fd(-42) {
	this->_writed = 0;
	this->_needToWrite = 0;
	this->_isDataHandled = false;
	this->_bufToSend = NULL;
}

Connection::Connection(int listenner, int fd) : _listennerFd(listenner), _fd(fd) {
	this->_writed = 0;
	this->_needToWrite = 0;
	this->_isDataHandled = false;
}

Connection::Connection(Connection const &other) : _listennerFd(other._listennerFd),
	_fd(other._fd), _writed(other._writed)
	{

}

Connection & Connection::operator=(Connection const &other) {
	if (this != &other) {
		this->_listennerFd = other._listennerFd;
		this->_fd = other._fd;
	}
	return *this;
}

int Connection::getFd() const {
	return this->_fd;
}

bool isOver(std::string req) {
	int last = req.length() - 1;
	if (req.length() < 4) {
		return false;
	}
	if (req[last] == 10 && req[last - 1] == 13
	 	&& req[last - 2] == 10 && req[last - 3] == 13) {
		return true;
	}
	return false;
}

int Connection::receiveData() {
	char buf[BUFFER];
	bzero(buf, BUFFER);
	int ret = recv(this->_fd, buf, BUFFER, 0);
	if (ret == -1) {
		std::cerr << this->_fd << " ";
		perror("recv");
		return -1;
	}
	if (ret == 0) {
		return 0;
	}
	this->buffer_in.append(buf, ret);
	if (isOver(this->buffer_in)) {
		return 0;
	}
	return ret;
}

void Connection::handleRequest() {
	std::cout << "handle request "<< std::endl;
	// for (int i = 0; i < (int) this->buffer_in.length(); i++) {
	// 	std::cout << (int) this->buffer_in[i] << " ";
	// }
	// std::cout << std::endl;
	// std::cout << "handle request"  << std::endl;
	// this->buffer_out.append("response: ");
	// this->buffer_out.append(this->buffer_in); // change to real handle
	this->buffer_out = "HTTP/1.1 200 OK\n\
	Date: Mon, 27 Jul 2009 12:28:53 GMT\n\
	Server: huyaache/2.2.14 (Win32)\n\
	Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n\
	Content-Length: 88\n\
	Content-Type: text/html\n\
	Connection: Closed\n\n\
	<html>\n\
	<body>\n\
	<h1>Hello, World!</h1>\n\
	<div>\n\
    <img src=\"/something.jpg\" alt="" />\n\
	</div>\n\
	</body>\n\
	</html>\n";
 	this->_needToWrite = this->buffer_out.length();
	this->_bufToSend = (char *) this->buffer_out.c_str();
	// this->buffer_out.clear();
	this->_isDataHandled = true;
	this->buffer_in.clear();
}


int Connection::sendData() {
	if (this->_writed >= this->_needToWrite) {
		this->_isDataHandled = false;
		this->_writed = 0;
		this->_needToWrite = 0;
		this->buffer_out.clear();
		return 0;
	}
	int bufferSize = this->_needToWrite - this->_writed > BUFFER ? BUFFER 
		: this->_needToWrite - this->_writed;
	int ret = send(this->_fd, this->_bufToSend + this->_writed, bufferSize, 0); // MSG_MORE FLAG?
	// check how its broken on buff 1
	if (ret == -1) {
		perror("send");
		return -1;
	}
	this->_writed += ret;
	if (this->_writed >= this->_needToWrite) {
		return 0;
	}
	return ret;
}


bool Connection::getHandleStatus() const {
	return this->_isDataHandled;
}

bool Connection::isReading(void) const {
	return !(this->buffer_in.empty());
}


Connection::~Connection(void) {}