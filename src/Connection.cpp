#include <unistd.h>
#include "Connection.hpp"

Connection::Connection(int listenner, int fd, VHost& vH) : _listennerFd(listenner),
				_fd(fd), _vHost(&vH) {
	this->_writed = 0;
	this->_needToWrite = 0;
	this->_request.setFd(fd);
	this->currentLoc = NULL;
	this->cgiIput.append(CGI_FILE_IN_PREFIX + std::to_string(fd));
	this->cgiOutput.append(CGI_FILE_OUT_PREFIX + std::to_string(fd));
	this->inputFilePost.append(INPUT_FILE_POST + std::to_string(fd));
	this->defaultErrorPageName.append(DEFAULT_ERROR_PAGE_PREFIX + std::to_string(fd) + ".html");
	this->inputFileFd = -1;
	this->bodyRecieved = 0;
	this->lastChunkSize = -1;
	this->currentChunkNotEnded = false;
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
		this->inputFileFd = other.inputFileFd;
		this->inputFilePost = other.inputFilePost;
		this->cgiOutput = other.cgiOutput;
		this->defaultErrorPageName = other.defaultErrorPageName;
		this->bodyRecieved = other.bodyRecieved;
		this->lastChunkSize = other.lastChunkSize;
		this->currentChunkNotEnded = other.currentChunkNotEnded;
		// add others
	}
	return *this;
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
	if (this->currentLoc->isCgi()) {
		Cgi::preprocessCgi(*this);
	} else if (this->_request.getParamByName("Method").compare("POST") == 0) {
		remove(this->inputFilePost.c_str());
		int fd = open(this->inputFilePost.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 777);
		if (fd == -1) {
			std::cerr << " input file not created" << std::endl;
			return ;
		}
		this->setFileInputFd(fd);
		this->sendBodyToFile();
	}
}

void Connection::unchunkBuffer() {
	std::string	newBuff;
	std::string	line;
	size_t		startPos = 0;

	if (this->_request.getParamByName("Transfer-Encoding") != "chunked") {
		return ;
	}
	// currentChunkNotEnded
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
	this->buffer_in.append(buf, ret);
	if (this->inputFileFd != -1) { // if post?  // body only here???
		this->unchunkBuffer();
		this->sendBodyToFile();
	}
	std::cout << "-----------buffer-in (recv)-------------" << std::endl;
	std::string tmp;
	tmp.append(buf, ret);
	std::cout << tmp << std::endl;
	std::cout << "-----------buffer-in-end--------------" << std::endl;
	if (ret == -1) {
		std::cerr << this->_fd << " ";
		perror("recv");
		return -1;
	}
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
		remove(this->defaultErrorPageName.c_str());
		if (this->currentLoc && this->currentLoc->isCgi()) {
			remove(this->cgiIput.c_str());
			remove(this->cgiOutput.c_str());
		}
		remove(this->inputFilePost.c_str());
		bzero(&this->routeObj, sizeof(this->routeObj));
		this->_writed = 0;
		this->_needToWrite = 0;
		this->buffer_in.clear();
		bzero(&(this->routeObj), sizeof(this->routeObj));
		this->currentLoc = NULL;
		this->inputFileFd = -1;
		this->bodyRecieved = 0;
		this->lastChunkSize = -1;
		if (this->_request.getCurrentCode() >= 400) {
			return -1;
		}
		this->_request.resetObj();
		this->_responce.resetObj();
		if (!this->_request.getParamByName("Connection").compare("close")) {
			return -1;
		}
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
		std::ofstream ofs;
		remove(this->defaultErrorPageName.c_str());
		ofs.open(this->defaultErrorPageName, std::ostream::out | std::ostream::trunc);
		if (ofs.is_open()) {
			page = getDefaultErrorPage(code);
			ofs << page;
			ofs.close();
			pageName = this->defaultErrorPageName;
		}
	}
	return pageName;
}

void	Connection::setResponceFile() {
	if (this->_request.getCurrentCode() == 0) {
		std::cout << "HTTP not found in set Responce" << std::endl;
		this->_request.setCurrentCode(505);
	}
	if (this->_request.getCurrentCode() >= 400) { // set erorr page
		this->_request.setFileNameToSend(this->getErrorPageName(this->getCurrectCode()));
	}

	if (!this->_responce.prepareFileToSend(this->_request.getFileToSend())) {
		std::cerr << "file not open: " << this->_request.getFileToSend() << std::endl;
		if (this->_request.getCurrentCode() >= 400) {
			this->_responce.setCode(this->_request.getCurrentCode());
			return ;
		}
		this->_request.setCurrentCode(404);
		this->_responce.prepareFileToSend(this->getErrorPageName(this->getCurrectCode()));
	}
	this->_responce.setCode(this->_request.getCurrentCode());
}

void GET(Request& request, routeParams &paramObj) {
	request.setFileNameToSend(paramObj.finalPathToFile);
}

void Connection::POST() {
	std::ifstream ifs;
	std::ofstream ofs;
	
	std::string conLength = this->_request.getParamByName("Content-Length");
	std::string	encoding = this->_request.getParamByName("Transfer-Encoding");

	if ( (conLength.empty() || std::stoi(conLength) == 0) && encoding.empty()) {
		return ; // change code? 
	}
	ifs.open(this->inputFilePost);
	// name == route? 
	ofs.open("TMPFILENAME", std::ofstream::out | std::ofstream::trunc); // where get file name? 
	if (ofs.is_open() && ifs.is_open()) {
		ofs << ifs.rdbuf();
	} else {
		this->setCurrentCode(500);
	}
	ofs.close();
	ifs.close();
}

void Connection::executeOrder66() { // all data recieved
	if (this->getCurrectCode() >= 400) {
		return ;
	}
	if (this->currentLoc && this->currentLoc->isCgi()) {
		std::cout << "CGI" << std::endl;
		close(this->inputFileFd);
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
			this->setCurrentCode(696);
			this->POST();
		}
		else if (!method.compare("DELETE")) {
			std::cout << "Method DELETE" << std::endl;
		}
	}
}

void Connection::prepareResponceToSend() {
	std::string	cgiHeader;
	bool		isCgiHeaderValid = false;
	this->setResponceFile();
	// if cgi take header from output. send to header obj
	if (this->currentLoc && this->currentLoc->isCgi()
		&& this->getCurrectCode() < 400) {
		cgiHeader = this->_responce.getCgiHeader();
		this->_responce.setCgiHeaderToResponce(cgiHeader, isCgiHeaderValid);
		if (cgiHeader.empty() || !isCgiHeaderValid) {
			this->setCurrentCode(502);
			this->setResponceFile();
		}
		// send cgiHeader to header. fill some fields
	}
	this->buffer_in.clear();
	this->_responce.createHeader(this->currentLoc);
	this->_needToWrite = this->_responce.getFileSize()
		+ this->_responce.getHeaderSize();
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
	write(this->inputFileFd, this->buffer_in.c_str(), this->buffer_in.length());
	this->bodyRecieved += this->buffer_in.length();
	this->buffer_in.clear();
}

Connection::~Connection(void) {}
Request& Connection::getRequestObj() {return this->_request; }
Responce& Connection::getResponceObj() {return this->_responce; }
int Connection::getListener() const {return this->_listennerFd; }


// Responce::getCgiHeader (from output)
// Responce::fillSomeFieldsInHeaderObj(cgiHeader)
// in createHeader: HeaderObj::getHeaderStr(loc, responceObj)