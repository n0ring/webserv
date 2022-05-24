#include "Connection.hpp"

#define BUFFER 2048

Connection::Connection(void) : _listennerFd(0), _fd(0) {
	this->_writed = 0;
	this->_isStartRead = false;
	this->_needToWrite = 0;
	this->_isDataHandled = false;
}

Connection::Connection(int listenner, int fd) : _listennerFd(listenner), _fd(fd) {
	this->_writed = 0;
	this->_isStartRead = false;
	this->_needToWrite = 0;
	this->_isDataHandled = false;
}

Connection::Connection(Connection const &other) : _listennerFd(other._listennerFd),
	_fd(other._fd), _writed(other._writed), _isStartRead(other._isStartRead) {

}

Connection & Connection::operator=(Connection const &other) {
	if (this != &other) {
		this->_listennerFd = other._listennerFd;
		this->_fd = other._fd;
		this->_isStartRead = other._isStartRead;
	}
	return *this;
}

int Connection::getFd() const {
	return this->_fd;
}

int Connection::receiveData() {
	char buf[BUFFER];
	bzero(buf, BUFFER);
	int ret = recv(this->_fd, buf, BUFFER, 0);
	std::cout << "recieved: " << ret << std::endl;
	if (ret == -1) {
		perror("recv");
		return -1;
	}
	if (ret == 0) {
		return 0;
	}
	this->buffer_in.append(buf, ret);
	if (this->_isStartRead == false) {
		this->_isStartRead = true;
	}
	return ret;
}

void Connection::handleRequest() {
	std::cout << "handle-> " << this->buffer_in << std::endl;
	// this->buffer_out = this->buffer_in;
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
	</body>\n\
	</html>\n";
 	this->_needToWrite = this->buffer_out.length();
	this->buffer_in.clear();
	this->_bufToSend = (char *) this->buffer_out.c_str();
	this->_isDataHandled = true;
	this->resetReadStatus();
}


int Connection::sendData() {
	if (this->_writed >= this->_needToWrite) {
		this->buffer_out.clear();
		this->resetHandleStatus();
		this->_writed = 0;
		this->_needToWrite = 0;
		return 0;
	}
	int bufferSize = this->_needToWrite - this->_writed > BUFFER ? BUFFER 
		: this->_needToWrite - this->_writed;
	int ret = send(this->_fd, this->_bufToSend + this->_writed, bufferSize, 0); // MSG_MORE
	if (ret == -1) {
		perror("send");
		return -1;
	}
	std::cout << "sended " << ret << std::endl;
	this->_writed += ret;
	return ret;
}

bool Connection::getReadStatus() const {
	return this->_isStartRead;
}


bool Connection::getHandleStatus() const {
	return this->_isDataHandled;
}


void	Connection::resetReadStatus() {
	this->_isStartRead = false;
}

void	Connection::resetHandleStatus() {
	this->_isDataHandled = false;
}





Connection::~Connection(void) {}