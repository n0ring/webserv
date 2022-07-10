#include "Header.hpp"

Header::Header(void) {}

Header::~Header(void) {
}


Header::Header(Header const& other) {
	*this = other;
}

Header&	Header::operator=(Header const& other) {
	if (this != &other) {
		this->status = other.status;
		this->contentType = other.contentType;
		this->contentLength = other.contentLength;
		this->connectionStatus = other.contentLength;
		this->unknownParams = other.unknownParams;
	}
	return *this;
}


void Header::appendToHeaderStr(std::string& param) {
	if (!param.empty()) {
		this->result.append(param + "\n");
	}
}


std::string	Header::getHeaderStr(void) {
	this->appendToHeaderStr(this->status);
	this->appendToHeaderStr(this->contentType);
	this->appendToHeaderStr(this->contentLength);
	this->appendToHeaderStr(this->connectionStatus);
	for (size_t i = 0; i < this->unknownParams.size(); i++) {
		this->appendToHeaderStr(this->unknownParams[i]);
	}
	this->result.append("\n");
	return this->result;
}

void	Header::reset(void) {
	this->status.clear();
	this->contentType.clear();
	this->contentLength.clear();
	this->connectionStatus.clear();
	this->unknownParams.clear();
	this->result.clear();
}

void	Header::setStatus(std::string nstatus) {
	if (this->status.empty()) {
		this->status = nstatus;
	}
}

void	Header::setContentType(std::string nContentType) {
	if (this->contentType.empty()) {
		this->contentType = nContentType;
	}
}

void	Header::setContentLength(std::string nContentLength) {
	if (this->contentLength.empty()) {
		this->contentLength = nContentLength;
	}
}

void	 Header::setConnectionStatus(std::string nConnectionStatus) {
	if (this->connectionStatus.empty()) {
		this->connectionStatus = nConnectionStatus;
	}
}

void Header::setHeaderParam(std::vector<std::string>& currentParams, std::string line) {
	std::cout << "header params: " << line << std::endl;
	if (currentParams.front() == "HTTP/1.1") {
		this->status = line;
	}
	else if (currentParams.front() == "Content-Type:") {
		this->contentType = line;
	}
	else if (currentParams.front() == "Content-Length:") {
		this->contentLength = line;
	}
	else if (currentParams.front() == "Connection:") {
		this->connectionStatus = line;
	}
	else {
		this->setParam(line);
	}
}

void	Header::setParam(std::string param) {
	this->unknownParams.push_back(param);
}

void	Header::checkCgiHeader(std::string& cgiHeader) {
	size_t						startPos = 0;
	std::string					line;
	std::vector<std::string>	currentParams;

	while (startPos < cgiHeader.length()) {
		line = getLine(cgiHeader, startPos);
		currentParams = sPPlit(line);
		setHeaderParam(currentParams, line);
	}
}


// get line 
// check for match 
