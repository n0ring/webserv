#include "Responce.hpp"

void Responce::setHeader(std::string header) {
	this->_header = header;
}

bool Responce::prepareFileToSend(std::string fileName) {
	this->ifs.open(fileName, std::ifstream::in);
	if (this->ifs.is_open() == false) {
		return false;
	}
	this->ifs.seekg (0, ifs.end);
	this->fileLen = this->ifs.tellg();
	ifs.seekg(0, ifs.beg);
	this->fileExtToSend = fileName.substr(fileName.find_last_of(".") + 1);
	std::cout << "filelen: " << this->fileLen << std::endl;
	if (this->fileLen == 0)  {
		return false;
	}
	return true;
}

size_t Responce::getFileSize() {
	return this->fileLen;
}

size_t Responce::getHeaderSize() {
	return this->_header.length();
}

size_t Responce::fillBuffer(char *buf) {
	size_t shiftHead = 0;
	size_t shiftBody = this->bodySended;

	if (this->headerSended < this->getHeaderSize()) {
		this->headerSended += shiftHead = this->_header.copy(buf, BUFFER);
	}
	if (this->ifs.is_open() && shiftHead < BUFFER && this->bodySended < this->fileLen) {
		this->ifs.read(buf + shiftHead, BUFFER - shiftHead);
		this->bodySended = this->ifs.eof() ? this->fileLen : (size_t) this->ifs.tellg(); 
	}
	return shiftHead + (this->bodySended - shiftBody);
}


void Responce::resetObj() {
	this->_header.clear();
	this->ifs.close();
	this->fileLen = 0;
	this->headerSended = 0;
	this->bodySended = 0;
	this->code = 0;
	this->contentLength = 0;
	this->contentType.clear();
	this->MIME.clear();
	this->fileExtToSend.clear();
}

void Responce::setCode(int c) {
	this->code = c;
}

void Responce::setHeaderStatus(void) {
	this->_header.append("HTTP/1.1 " + std::to_string(this->code) + "OK\n");
}


// get header from file
// check whats filled 
// fill the others



void Responce::setHeaderFromFile(std::string& cgiHeader) {
	char buf[BUFFER];
	while (this->ifs.rdstate() != std::ifstream::failbit) {
		bzero(buf, BUFFER);
		this->ifs.getline(buf, BUFFER);
		if (this->ifs.rdstate() & std::ifstream::failbit || buf[0] == '\0') {
			break;
		}
		cgiHeader.append(buf);
		cgiHeader.append("\n");
	}
}

void Responce::createCGiHeader(void) {
	std::string cgiHeader;
	if (!this->ifs.is_open()) {
		return ;
	}
	this->setHeaderFromFile(cgiHeader);
	std::cout << "CGI HEADER: " << cgiHeader << std::endl;
	

	ifs.seekg(0, ifs.beg);
}

void Responce::createHeader(location* loc) {
	Mime::set(this->fileExtToSend, this->MIME);
	if (loc->isCgi() && this->code < 400) {
		this->createCGiHeader();
		return ;
	}
	this->setHeaderStatus();

	// this->_header.append("Date: Mon, 27 Jul 2009 12:28:53 GMT\n\
	// Server: huyaache/2.2.14 (Win32)\n\
	// Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n");
	this->_header.append("Content-Length: ");
		this->_header.append(std::to_string(this->fileLen) + "\n");
	this->_header.append("Connection: Close\n");
	if (  !(loc && loc->isCgi()) || code >= 400) { // > 500? 
		if (!this->MIME.empty()) {
			this->_header.append("Content-Type: " + this->MIME + "\n");
		}
		this->_header.append("\n");
	}
	std::cout << GREEN << "-----header to send------" << std::endl;
	std::cout << this->_header << std::endl << RESET;
}