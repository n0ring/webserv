#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int listenner, int fd, VHost& vH, Utils* nUtils) : 
		_responce(nUtils),
		inputBufferName(INPUT_FILE_POST + std::to_string(fd)),
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
	this->utils = nUtils;
}

Connection::Connection(Connection const &other) : _responce(other.utils),
				inputBufferName(other.inputBufferName),
				cgiOutput(other.cgiOutput) {
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
	this->utils = other.utils;
}

Connection::~Connection(void) {
	if (this->ofs.is_open()) {
		this->ofs.close();
	}
	remove(this->inputBufferName.c_str());
	remove(this->cgiOutput.c_str());
}


int Connection::getFd() const {
	return this->_fd;
}

void Connection::processLocation() {
	int contentLen;

	stringToNum(this->_request.getParamByName("Content-Length"), contentLen);

	if (contentLen > this->_vHost->getMaxBody()) {
		this->_request.setCurrentCode(CODE_REQUEST_ENTITY_TOO_LARGE);
		return ;
	}
	if (this->currentLoc == NULL) {
		this->_request.setCurrentCode(CODE_NOT_FOUND);
		return ;
	}
	if (this->currentLoc->isMethodAllow(this->_request.getParamByName("Method")) == false) {
		this->_request.setCurrentCode(CODE_METHOD_NOT_ALLOWED);
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
	this->_request.setCurrentCode(CODE_OK);
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
			stringToNum(line, this->lastChunkSize, true);
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
	std::cout << "Unchucnked: " << newBuff << std::endl;
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
		// perror("send error: ");
		// std::cout << RED  << this->_request.getFileToSend() <<  " sended-FAIL" << RESET << std::endl;
		return -1;
	}
	this->_writed += sended;
	if (this->_writed >= this->_needToWrite) { // end of sending 
		// std::cout << GREEN <<  this->_request.getFileToSend()
		// << " sended-OK" << RESET << std::endl;
		if (this->_request.getParamByName("Connection") == "close" 
				|| (this->currentLoc && this->currentLoc->isCgi())
				|| this->_request.getCurrentCode() >= 400 ) {
			return -1;
		}
		if (this->_request.getParamByName("Method") == "GET"
			&& !this->_responce.getFileSize()
			&& this->bodyOut.empty()) {
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
		this->bodyOut = this->utils->getDefaultErrorPage(code);
	}
	return pageName;
}



void Connection::GET() {
	std::string route = this->_request.getParamByName("Route");
	if (this->currentLoc && route == this->currentLoc->getLocationName()
			&& this->currentLoc->getParamByName("autoindex") == "on") {
		this->bodyOut = FileList::getFileListHTML(this->currentLoc->getParamByName("root"), route);
	} else {
		this->_request.setFileNameToSend(this->routeObj.finalPathToFile);
	}
}

void Connection::POST() {
	std::ifstream	ifs;
	std::ofstream	ofss;
	std::string		uploadPath;
	int				conLength; 
	std::string		encoding = this->_request.getParamByName("Transfer-Encoding");
	std::string		responceFileName = this->currentLoc->getParamByName("root")
						+ "/" + this->currentLoc->getParamByName("index");

	stringToNum(this->_request.getParamByName("Content-Length"), conLength);
	if (!this->currentLoc) {
		this->setCurrentCode(CODE_NOT_FOUND);
		return ;
	}
	uploadPath = this->currentLoc->getParamByName("upload");
	if (uploadPath.empty()) {
		this->setCurrentCode(CODE_BAD_REQUEST);
		return ;
	}
	if (conLength == 0 && encoding.empty()) {
		this->setCurrentCode(CODE_LENGTH_REQUIRED);
		return ; // change code? 
	}
	this->ofs.close();
	ifs.open(this->inputBufferName);
	// name == route? 
	ofss.open(uploadPath, std::ofstream::out | std::ofstream::trunc); // where get file name? 
	if (ofss.is_open() && ifs.is_open()) {
		ofss << ifs.rdbuf();
	} else {
		this->setCurrentCode(CODE_INTERNAL_SERVER_ERROR);
		return ;
	}
	this->_request.setFileNameToSend(responceFileName);
	this->setCurrentCode(CODE_CREATED);
	ofss.close();
	ifs.close();
}

void Connection::executeOrder66() { // all data recieved
	if (this->getCurrectCode() >= 400) {
		return ;
	}
	if (this->currentLoc && this->currentLoc->isCgi()) {
		// std::cout << "CGI" << std::endl;
		remove(this->cgiOutput.c_str());
		this->ofs.close();
		int ret = Cgi::start(*this->currentLoc, this->inputBufferName, this->cgiOutput, this->_request);
		if (ret == -42) {
			this->setCurrentCode(CODE_GATEWAY_TIMEOUT);
			return ;
		}
		if (ret != 0) {
			this->setCurrentCode(CODE_INTERNAL_SERVER_ERROR);
		}
		this->_request.setFileNameToSend( this->cgiOutput.c_str());
	}
	else {
		std::string method = this->_request.getParamByName("Method");
		if (!method.compare("GET")) {
			// std::cout << "Method GET" << std::endl;
			this->GET();
		}
		else if (!method.compare("POST")) {
			// std::cout << "Method POST" << std::endl;
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
		this->_request.setCurrentCode(CODE_HTTP_VERSION_NOT_SUPPORTED);
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
		this->_request.setCurrentCode(CODE_NOT_FOUND);
		this->_responce.prepareFileToSend(this->getErrorPageName(this->getCurrectCode()), this->bodyOut);
	}
	this->_responce.setCode(this->_request.getCurrentCode());
}


void Connection::prepareResponceToSend() {
	std::string	cgiHeader;

	this->setResponceFile();
	if (this->currentLoc && this->currentLoc->isCgi()
		&& this->getCurrectCode() != CODE_GATEWAY_TIMEOUT) {
		cgiHeader = this->_responce.getCgiHeader();
		this->_responce.setCgiHeaderToResponce(cgiHeader);
		if (cgiHeader.empty()) {
			this->setCurrentCode(CODE_BAD_GATEWAY);
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
		this->setCurrentCode(CODE_INTERNAL_SERVER_ERROR);
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
	remove(this->cgiOutput.c_str());
}

int		Connection::getCurrectCode(void) { return this->_request.getCurrentCode(); }
void	Connection::setCurrentCode(int fd) { this->_request.setCurrentCode(fd);}
