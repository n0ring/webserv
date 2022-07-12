#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int listenner, int fd, VHost& vH) : 
		inputBufferName(INPUT_FILE_POST + std::to_string(fd)),
		defaultErrorPageName(DEFAULT_ERROR_PAGE_PREFIX + std::to_string(fd) + ".html"),
		cgiOutput(CGI_FILE_OUT_PREFIX + std::to_string(fd)) {
	this->_listennerFd = listenner;
	this->_fd = fd;
	this->_vHost = &vH;
	this->_writed = 0;
	this->_needToWrite = 0;
	this->currentLoc = NULL;
	this->bodyRecieved = 0;
	this->lastChunkSize = -1;
	this->currentChunkNotEnded = false;
}

Connection::Connection(Connection const &other) : inputBufferName(other.inputBufferName),
		defaultErrorPageName(other.defaultErrorPageName), cgiOutput(other.cgiOutput) {
	this->_listennerFd = other._listennerFd;
	this->_fd = other._fd;
	this->_vHost = other._vHost;
	this->routeObj = other.routeObj;
	this->currentLoc = other.currentLoc;
	this->_writed = other._writed;
	this->_needToWrite = other._needToWrite;
	this->buffer_in = other.buffer_in;
	this->bodyOut = other.bodyOut;
	this->bodyRecieved = other.bodyRecieved;
	this->lastChunkSize = other.lastChunkSize;
	this->currentChunkNotEnded = other.currentChunkNotEnded;
}

Connection::~Connection(void) {
	if (this->ofs.is_open()) {
		this->ofs.close();
	}
	remove(this->inputBufferName.c_str());
	remove(this->defaultErrorPageName.c_str());
	remove(this->cgiOutput.c_str());
}


int Connection::getFd() const {
	return this->_fd;
}

void Connection::processLocation() {
	int contentLen;

	stringToNum(this->_request.getParamByName("Content-Length"), contentLen);

	if (contentLen > this->_vHost->getMaxBody()) {
		this->_request.setCurrentCode(413);
		return ;
	}
	if (this->currentLoc == NULL) {
		this->_request.setCurrentCode(404);
		return ;
	}
	if (this->currentLoc->isMethodAllow(this->_request.getParamByName("Method")) == false) {
		this->_request.setCurrentCode(405);
		return ;
	}
	if (!this->currentLoc->getParamByName("redirect").empty()) {
		this->_request.setCurrentCode(this->currentLoc->getRedirectCode());
		this->_responce.setParamToHeader("Location: " + this->currentLoc->getParamByName("redirect"));
		return ;
	}
	if (this->_request.getParamByName("Transfer-Encoding") == "chunked") {
		this->unchunkBuffer();	
	}
	this->_request.setCurrentCode(200);
	if (this->currentLoc->isCgi() || this->_request.getParamByName("Method") == "POST") {
		this->preparaBufferForBody();
		this->saveBody();
	}
}

void Connection::unchunkBuffer() {
	std::string	newBuff;
	std::string	line;
	size_t		startPos = 0;

	if (this->_request.getParamByName("Transfer-Encoding") != "chunked") {
		return ;
	}
	while (startPos < this->buffer_in.length()) {
		if (!currentChunkNotEnded) {
			line = getLine(this->buffer_in, startPos);
			stringToNum(line, this->lastChunkSize);
			if (this->lastChunkSize == 0) {
				break; ;
			}
			while (this->buffer_in[startPos] == '\n' || this->buffer_in[startPos] == '\r') {
				startPos++;
			}
		}
		if (this->lastChunkSize < this->buffer_in.length() - startPos) { // all chunk in buffer
			newBuff.append(this->buffer_in, startPos, this->lastChunkSize);
			startPos += this->lastChunkSize;
			while (this->buffer_in[startPos] == '\n' || this->buffer_in[startPos] == '\r') {
				startPos++;
			}
			this->currentChunkNotEnded = false;
		}
		else { // only part of chunk in buffer
			this->currentChunkNotEnded = true;
			newBuff.append(this->buffer_in, startPos, this->buffer_in.length() - startPos);
			this->lastChunkSize -= (this->buffer_in.length() - startPos);
			if (this->lastChunkSize == 0) {
				this->lastChunkSize--;
			}
			break;
		}
	}
	this->buffer_in = newBuff;
}

int Connection::receiveData() {  // viHost
	char	buf[BUFFER];
	int		ret;
	bzero(buf, BUFFER);
	ret = recv(this->_fd, buf, BUFFER, 0);
	if (ret == -1) {
		std::cerr << this->_fd << " ";
		perror("recv");
		return -1;
	}
	this->buffer_in.append(buf, ret);
	if (this->ofs.is_open()) { // if post?  // body only here???
		this->unchunkBuffer();
		this->saveBody();
	}
	// std::cout << "-----------buffer-in (recv)-------------" << std::endl;
	// std::string tmp;
	// tmp.append(buf, ret);
	// std::cout << tmp << std::endl;
	// std::cout << "-----------buffer-in-end--------------" << std::endl;

	this->_request.setHeader(this->buffer_in);
	if (this->_request.getHeader().empty() == false && this->_request.getCurrentCode() == 0) {
		this->checkForVhostChange();
		this->_vHost->setLocation(this->_request, this->routeObj, &this->currentLoc);
		this->processLocation();
	}
	if (ret == 0) { return -1; }
	if (_request.getCurrentCode() >= 400 || (ret < BUFFER && !this->isMoreBody())) {
		return 0;
	}
	return ret;
}

bool Connection::isMoreBody(void) {
	int contentLength;
	
	stringToNum(this->_request.getParamByName("Content-Length"), contentLength);
	if (this->lastChunkSize == 0) { // if not chunk value -1 on start and dont change 
		return false;
	}
	if (this->bodyRecieved < contentLength) { // if contLen.empty() contentLength == 0
		return true;
	}
	return false;
}

int Connection::sendData() {
	char	buf[BUFFER];
	size_t	readyToSend = 0;
	int		sended;
	
	bzero(buf, BUFFER);

	readyToSend = this->_responce.fillBuffer(buf, this->bodyOut);
	sended = send(this->_fd, buf, readyToSend, 0); // MSG_MORE FLAG?
	if (sended == -1) {
		perror("send error: ");
		std::cout << RED  << this->_request.getFileToSend() <<  " sended-FAIL" << RESET << std::endl;
		return -1;
	}
	this->_writed += sended;
	if (this->_writed >= this->_needToWrite) { // end of sending 
		std::cout << GREEN <<  this->_request.getFileToSend()
		<< " sended-OK" << RESET << std::endl;
		if (this->_request.getParamByName("Connection") == "close" 
				|| (this->currentLoc && this->currentLoc->isCgi())
				|| this->_request.getCurrentCode() >= 400 ) {
			return -1;
		}
		this->resetConnection();
		return 0;
	}
	return sended;
}

std::string Connection::getErrorPageName(int code) {
	std::string pageName;
	std::string	page;

	if (this->currentLoc) {
		pageName = this->currentLoc->getErrorPage(code);
	}
	if (pageName.empty()) {
		pageName = this->_vHost->getErrorPage(code);
	}
	if (pageName.empty()) {
		this->bodyOut = getDefaultErrorPage(code);
	}
	return pageName;
}


void GET(Request& request, routeParams &paramObj) {
	request.setFileNameToSend(paramObj.finalPathToFile);
}

void Connection::POST() {
	std::ifstream	ifs;
	std::ofstream	ofss;
	std::string		uploadPath;
	int				conLength; 
	std::string	encoding = this->_request.getParamByName("Transfer-Encoding");

	stringToNum(this->_request.getParamByName("Content-Length"), conLength);
	if (!this->currentLoc) {
		this->setCurrentCode(404);
		return ;
	}
	uploadPath = this->currentLoc->getParamByName("upload");
	if (uploadPath.empty()) {
		this->setCurrentCode(999);
		std::cout << "Upload path is not define. You config suck" << std::endl;
		return ;
	}
	if (conLength == 0 && encoding.empty()) {
		this->setCurrentCode(999);
		return ; // change code? 
	}
	this->ofs.close();
	ifs.open(this->inputBufferName);
	// name == route? 
	ofss.open(uploadPath, std::ofstream::out | std::ofstream::trunc); // where get file name? 
	if (ofss.is_open() && ifs.is_open()) {
		ofss << ifs.rdbuf();
	} else {
		this->setCurrentCode(500);
	}
	this->_request.setFileNameToSend(this->routeObj.finalPathToFile);
	ofss.close();
	ifs.close();
}

void Connection::executeOrder66() { // all data recieved
	if (this->getCurrectCode() >= 400) {
		return ;
	}
	if (this->currentLoc && this->currentLoc->isCgi()) {
		std::cout << "CGI" << std::endl;
		remove(this->cgiOutput.c_str());
		this->ofs.close();
		if (Cgi::start(*this->currentLoc, this->inputBufferName, this->cgiOutput, this->_request)) {
			this->setCurrentCode(500);
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
			this->POST();
		}
		else if (!method.compare("DELETE")) {
			std::cout << "Method DELETE" << std::endl;
		}
	}
}

void	Connection::setResponceFile() {
	if (this->_request.getCurrentCode() == 0) {
		std::cout << "HTTP not found in set Responce" << std::endl;
		this->_request.setCurrentCode(505);
	}
	if (this->_request.getCurrentCode() >= 400 && this->_request.getFileToSend().empty()) { // set erorr page
		this->_request.setFileNameToSend(this->getErrorPageName(this->getCurrectCode()));
	}
	// check is it a file
	if (!this->_responce.prepareFileToSend(this->_request.getFileToSend(), this->bodyOut)) {
		std::cerr << "file not open: " << this->_request.getFileToSend() << std::endl;
		if (this->_request.getCurrentCode() >= 400) {
			this->_responce.setCode(this->_request.getCurrentCode());
			return ;
		}
		this->_request.setCurrentCode(404);
		this->_responce.prepareFileToSend(this->getErrorPageName(this->getCurrectCode()), this->bodyOut);
	}
	this->_responce.setCode(this->_request.getCurrentCode());
}


void Connection::prepareResponceToSend() {
	std::string	cgiHeader;

	this->setResponceFile();
	if (this->currentLoc && this->currentLoc->isCgi()) {
		cgiHeader = this->_responce.getCgiHeader();
		this->_responce.setCgiHeaderToResponce(cgiHeader);
		if (cgiHeader.empty()) {
			std::cout << "here shit some" << std::endl;
			this->setCurrentCode(502);
			this->_request.setFileNameToSend("");
			this->setResponceFile();
		}
	}
	this->buffer_in.clear();
	this->_responce.createHeader(this->currentLoc);
	this->_needToWrite = this->_responce.getHeaderSize();
	if (this->_request.getFileToSend().empty()) {
		this->_needToWrite += this->bodyOut.size();
	} 
	else {
		this->_needToWrite += this->_responce.getFileSize();	
	}
	
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

void	Connection::saveBody() {
	if (this->getCurrectCode() >= 400) {
		return ;
	}
	this->ofs << this->buffer_in;
	this->bodyRecieved += this->buffer_in.length();
	this->buffer_in.clear();
}

Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }


void	Connection::preparaBufferForBody() {
	remove(this->inputBufferName.c_str());
	this->ofs.open(this->inputBufferName, std::ofstream::out);
	if (!this->ofs.is_open()) {
		perror("create input file fail");
		this->setCurrentCode(500);
		return ;
	}
}

void	Connection::resetConnection(void) {
	this->_request.resetObj();
	this->_responce.resetObj();
	bzero(&this->routeObj, sizeof(this->routeObj));
	this->currentLoc = NULL;
	this->_writed = 0;
	this->_needToWrite = 0;
	this->buffer_in.clear();
	this->bodyRecieved = 0;
	this->lastChunkSize = -1;
	this->currentChunkNotEnded = false;
	this->bodyOut.clear();
	if (this->ofs.is_open()) {
		this->ofs.close();
	}
	remove(this->inputBufferName.c_str());
	remove(this->defaultErrorPageName.c_str());
	remove(this->cgiOutput.c_str());
}
