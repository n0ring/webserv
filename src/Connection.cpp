#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int listenner, int fd, VHost& vH) : _listennerFd(listenner),
				_fd(fd), _vHost(vH) {
	this->_writed = 0;
	this->_needToWrite = 0;
}

Connection::Connection(Connection const &other) : _listennerFd(other._listennerFd),
	_fd(other._fd),  _vHost(other._vHost), _writed(other._writed)
	{
}

Connection & Connection::operator=(Connection const &other) {
	if (this != &other) {
		this->_listennerFd = other._listennerFd;
		this->_fd = other._fd;
		this->_vHost = other._vHost;
	}
	return *this;
}

int Connection::getFd() const {
	return this->_fd;
}

bool isRecieveOver(std::string req) { // need to refactor or delete
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

/*
During receive data need to set and process header. 
after pricess header set params for receive next data pack
(place to save, continue or not)
*/
int Connection::receiveData() {  // viHost
	char buf[BUFFER];
	int ret;
	
	ret = recv(this->_fd, buf, BUFFER, 0);
	if (false) {
		// save buffer to file. only with POST and file content
	}
	else {
		this->buffer_in.append(buf, ret);
	}
	if (ret == -1) {
		std::cerr << this->_fd << " ";
		perror("recv");
		return -1;
	}
	this->_request.setHeader(this->buffer_in);
	if (this->_request.getHeader().empty() == false 
		&& this->_request.getCurrentCode() == 0) {
			this->_vHost.processHeader(this->_request);
	}
	if (this->_request.getCurrentCode() >= 400 
		|| ret < BUFFER
		|| isRecieveOver(this->buffer_in)) {
			this->_vHost.setResponce(this->_request, this->_responce);
			this->prepareResponceToSend();
		return 0;
	}
	return ret;
}

void Connection::prepareResponceToSend() {
	this->buffer_in.clear();
	this->_responce.createHeader();
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
	if (this->_writed >= this->_needToWrite) { // end of sending 
		this->_responce.resetObj();
		this->_writed = 0;
		this->_needToWrite = 0;
		this->_request.resetObj();
		if (this->_request.getCurrentCode() >= 400) {
			return -1;
		}
		return 0;
	}
	return sended;
}


Connection::~Connection(void) {}
Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }