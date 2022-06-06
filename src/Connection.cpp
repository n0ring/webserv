#include "Connection.hpp"


Connection::Connection(void) : _listennerFd(-42), _fd(-42) {
	this->_writed = 0;
	this->_needToWrite = 0;
}

Connection::Connection(int listenner, int fd) : _listennerFd(listenner), _fd(fd) {
	this->_writed = 0;
	this->_needToWrite = 0;
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

bool isRecieveOver(std::string req) { // need to refactor
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
	bzero(buf, BUFFER); // delete
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
	if (isRecieveOver(this->buffer_in)) {
		this->_request.parseStr(this->buffer_in);
		return 0;
	}
	return ret;
}

void Connection::prepareResponceToSend() {
	this->buffer_in.clear();
	this->_needToWrite = this->_responce.getFileSize() + this->_responce.getHeaderSize();
}

int Connection::sendData() {
	char	buf[BUFFER];
	size_t	readyToSend = 0;
	int		sended;
	
	bzero(buf, BUFFER);
	
	readyToSend = this->_responce.fillBuffer(buf);
	sended = send(this->_fd, buf, readyToSend, 0); // MSG_MORE FLAG?
	if (sended == -1) {
		perror("send");
		return -1;
	}
	this->_writed += sended;
	if (this->_writed >= this->_needToWrite) {
		this->_writed = 0;
		this->_needToWrite = 0;
		this->_responce.closeFile();
		return 0;
	}
	return sended;
}


Connection::~Connection(void) {}
Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }