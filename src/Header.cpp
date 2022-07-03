#include "Header.hpp"

Header::Header(void) {}

Header::~Header(void) {}


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
		this->end = other.end;
	}
	return *this;
}

std::string	Header::getHeaderStr(void) {
	this->result.append(this->status + "\n");
	this->result.append(this->contentType + "\n");
	this->result.append(this->contentLength + "\n");
	this->result.append(this->connectionStatus + "\n");
	// this->result.append(this->unknownParams + "\n") /// 1!!!!!!!!!!
	this->result.append(this->end);
	return this->result;
}

void	Header::reset(void) {
	this->status.clear();
	this->contentType.clear();
	this->contentLength.clear();
	this->connectionStatus.clear();
	this->end.clear();
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

void	Header::setEndOfHeader(bool isNeed) {
	if (isNeed) {
		end = "\n";
	} else {
		end.clear();
	}
}

bool Header::isParamMatch(std::vector<std::string>& currentParams, std::string line) {
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
		this->unknownParams.append(line + "\n");
		return false;
	}
	return true;
}

bool	Header::validateParams(void) {
	if (this->contentType.empty()) {
		return false;
	}
	return true;
}


void	Header::checkCgiHeader(std::string& cgiHeader, bool& isCgiHeaderValid) {
	size_t						startPos = 0;
	std::string					line;
	std::vector<std::string>	currentParams;

	while (startPos < cgiHeader.length()) {
		line = getLine(cgiHeader, startPos);
		currentParams = sPPlit(line);
		if (isParamMatch(currentParams, line)) {
			isCgiHeaderValid = true;
		}
	}
	
	if (isCgiHeaderValid) {
		isCgiHeaderValid = validateParams();
	}
}


// get line 
// check for match 
