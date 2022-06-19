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
	if (!this->_request.getHeader().empty() && this->currentLoc->isCgi()) { // if post? 
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
		// if cgi open inputfile. send there all except header. next packege save there.
		if (this->currentLoc->isCgi()) {
			this->cgiIputFd = open(this->cgiIput.c_str(), O_RDWR | O_CREAT | O_TRUNC);
			if (this->cgiIputFd == -1) {
				perror("crete input file");
				this->_request.setCurrentCode(500);
				return 0;
			}
			write(this->cgiIputFd, this->body.c_str(), this->body.length());
		}
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

void	Connection::setResponce() {
	std::string errorFile = "www/errors/404.html";
	if (this->_request.getCurrentCode() == 0) {
		std::cout << "HTTP not found in set Responce" << std::endl;
		this->_request.setCurrentCode(505);
	}
	if (this->_request.getCurrentCode() >= 400) { // set erorr page
		this->_request.setFileNameToSend(errorFile);
	}
	if (!this->_responce.prepareFileToSend(this->_request.getFileToSend())) {
		std::cerr << "file not open: " << this->_request.getFileToSend() << std::endl;
		this->_request.setCurrentCode(404);
		this->_responce.prepareFileToSend(errorFile); // if can't open set default
	}
	this->_responce.setCode(this->_request.getCurrentCode());
}

void GET(Request& request, routeParams &paramObj) {
	request.setFileNameToSend(paramObj.finalPathToFile);
}

void Connection::executeOrder66() { // all data recieved
	if (this->currentLoc->isCgi()) {
		std::cout << "CGI" << std::endl;
		this->_request.setCgiPid(Cgi::start(*this->currentLoc, TMP_FILE, this->_request));
		this->_request.setFileNameToSend(TMP_FILE);
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
	this->executeOrder66();
	this->setResponce();
	this->buffer_in.clear();
	// if file to send not open set default.
	this->_responce.createHeader(this->_request.getCgiPid());
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
		bzero(&this->routeObj, sizeof(this->routeObj));
		this->_writed = 0;
		this->_needToWrite = 0;
		remove(this->cgiIput.c_str());
		remove(this->cgiOutput.c_str());
		if (this->_request.getCgiPid() > 0) {
			remove(TMP_FILE); // if cgi
		}
		if (this->_request.getCurrentCode() >= 400) {
			return -1;
		}
		// if connection: close return -1;
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

Connection::~Connection(void) {}
Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }