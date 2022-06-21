#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int listenner, int fd, VHost& vH) : _listennerFd(listenner),
				_fd(fd), _vHost(&vH) {
	this->_writed = 0;
	this->_needToWrite = 0;
	this->_request.setFd(fd);
	this->currentLoc = NULL;
	this->cgiIput.append(".cgi_input" + std::to_string(fd));
	this->cgiOutput.append(".cgi_output" + std::to_string(fd));
	this->cgiIputFd = -1;
	remove(this->cgiOutput.c_str());
	remove(this->cgiIput.c_str());
}

Connection::Connection(Connection const &other) {
	*this = other;
}

Connection & Connection::operator=(Connection const &other) {
	if (this != &other) {
		this->_listennerFd = other._listennerFd;
		this->_fd = other._fd;
		this->_vHost = other._vHost;
		this->_writed = other._writed;
		this->_needToWrite = other._needToWrite;
		this->currentLoc = other.currentLoc;
		this->cgiIput = other.cgiIput;
		this->cgiIputFd = other.cgiIputFd;
		this->cgiOutput = other.cgiOutput;
		// add others
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
after process header set params for receive next data pack
(place to save, continue or not)
*/
int Connection::receiveData() {  // viHost
	char buf[BUFFER];
	int ret;
	
	ret = recv(this->_fd, buf, BUFFER, 0);
	// if (!this->_request.getHeader().empty()) { // header !empty
	if (this->cgiIputFd != -1) { // if post? 
		write(this->cgiIputFd, buf, ret);
	}
	else {
		this->buffer_in.append(buf, ret);
	}
	// std::cout << "-----------buffer-in-----------------" << std::endl;
	// std::cout << this->buffer_in << std::endl;
	// std::cout << "-----------buffer-in-end--------------" << std::endl;
	if (ret == -1) {
		std::cerr << this->_fd << " ";
		perror("recv");
		return -1;
	}
	this->_request.setHeader(this->buffer_in);
	if (this->_request.getHeader().empty() == false && this->_request.getCurrentCode() == 0) {
		this->checkForVhostChange();
		this->_vHost->processHeader(this->_request, this->routeObj, &this->currentLoc);
		if (this->currentLoc && this->currentLoc->isCgi()) {
			Cgi::preprocessCgi(*this);
		} // only for good code
		// if cgi open inputfile. send there all except header. next packege save there
	}
	if (ret == 0) {
		return -1;
	}
	if (_request.getCurrentCode() >= 400 || ret < BUFFER || isRecieveOver(buffer_in)) {
		// if body not empty process body ?????
		return 0;
	}
	return ret;
}

std::string Connection::getErrorPageName(int code) {
	std::string pageName;
	if (this->currentLoc) {
		pageName = this->currentLoc->getErrorPage(code);
	}
	if (pageName.empty()) {
		pageName = this->_vHost->getErrorPage(code);
	}
	if (pageName.empty()) {
		return "www/errors/1.html";
	}
	return pageName;
}

void	Connection::setResponce() {
	if (this->_request.getCurrentCode() == 0) {
		std::cout << "HTTP not found in set Responce" << std::endl;
		this->_request.setCurrentCode(505);
	}
	if (this->_request.getCurrentCode() >= 400) { // set erorr page
		this->_request.setFileNameToSend(this->getErrorPageName(this->getCurrectCode()));
	}
	if (!this->_responce.prepareFileToSend(this->_request.getFileToSend())) {
		std::cerr << "file not open: " << this->_request.getFileToSend() << std::endl;
		this->_request.setCurrentCode(404);
		this->_responce.prepareFileToSend(this->getErrorPageName(this->getCurrectCode())); // if can't open set default
	}
	this->_responce.setCode(this->_request.getCurrentCode());
}

void GET(Request& request, routeParams &paramObj) {
	request.setFileNameToSend(paramObj.finalPathToFile);
}

void Connection::executeOrder66() { // all data recieved
	if (this->currentLoc && this->currentLoc->isCgi()) {
		std::cout << "CGI" << std::endl;
		close(this->cgiIputFd);
		if (Cgi::start(*this->currentLoc, this->cgiIput, this->cgiOutput, this->_request)) {
			this->setCurrentCode(500);
			return ;
		}
		this->_request.setFileNameToSend( this->cgiOutput.c_str());
	}
	else {
		std::string method = this->_request.getParamByName("Method");
		if (!method.compare("GET")) {
			std::cout << "Method GET" << std::endl;
			GET(this->_request, this->routeObj);
		}
		else if (!method.compare("POST")) {
			std::cout << "Method POST" << std::endl;
		}
		else if (!method.compare("DELETE")) {
			std::cout << "Method DELETE" << std::endl;
		}
	}
}

void Connection::prepareResponceToSend() {
	if (this->getCurrectCode() < 400) {
		this->executeOrder66();
	} 
	this->setResponce();
	this->buffer_in.clear();
	// if file to send not open set default.
	this->_responce.createHeader(this->currentLoc);
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
		std::cout << RED  << this->_request.getFileToSend() <<  " sended-FAIL" << RESET << std::endl;
		return -1;
	}
	this->_writed += sended;
	if (this->_writed >= this->_needToWrite) { // end of sending 
		std::cout << GREEN <<  this->_request.getFileToSend() << " sended-OK" << RESET << std::endl;
		if (!this->_request.getParamByName("Connection").compare("close")) {
			return -1;
		}
		if (this->currentLoc && this->currentLoc->isCgi()) {
			remove(this->cgiIput.c_str());
			remove(this->cgiOutput.c_str());
			// return -1;
		}
		bzero(&this->routeObj, sizeof(this->routeObj));
		this->_writed = 0;
		this->_needToWrite = 0;
		if (this->_request.getCurrentCode() >= 400) {
			return -1;
		}
		this->_request.resetObj();
		this->_responce.resetObj();
		return 0;
	}
	return sended;
}

void	Connection::checkForVhostChange() {
	VHost* newVhost = NULL;
	std::string s = this->_request.getParamByName("Host");
	std::vector<std::string> hostParams;
	splitByChar(s, ':', hostParams);
	if (hostParams.empty()) {
		return ;
	}
	newVhost = this->_vHost->changeVhost(hostParams.front());
	if (newVhost) {
		this->_vHost = newVhost;
	}
}

void	Connection::sendBodyToFile() {
	write(this->cgiIputFd, this->buffer_in.c_str(), this->buffer_in.length());
}

Connection::~Connection(void) {}
Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }