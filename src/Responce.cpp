#include "Responce.hpp"

void Responce::setHeader(std::string header) {
	this->_header = header;
}

bool Responce::prepareFileToSend(const char *fileName) {
	this->ifs.open(fileName, std::ifstream::in);
	if (this->ifs.is_open() == false) {
		return false;
	}
	this->ifs.seekg (0, ifs.end);
    this->fileLen = this->ifs.tellg();
    ifs.seekg(0, ifs.beg);
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
	if (shiftHead < BUFFER && this->bodySended < this->fileLen) {
		this->ifs.read(buf + shiftHead, BUFFER - shiftHead);
		this->bodySended = this->ifs.eof() ? this->fileLen : (size_t) this->ifs.tellg(); 
	}
	return shiftHead + (this->bodySended - shiftBody);
}


void Responce::resetObj() {
	this->_header.clear();
	this->_body.clear();
	this->ifs.close();
	this->fileLen = 0;
	this->headerSended = 0;
	this->bodySended = 0;
	this->code = 0;
	this->contentLength = 0;
	this->contentType.clear();
}

void Responce::setCode(int c) {
	this->code = c;
}




void Responce::createHeader(void) {
	this->_header.append("HTTP/1.1");
	this->_header.append(" ");
	this->_header.append(std::to_string(this->code));
	this->_header.append(" ");
	this->_header.append("OK");
	this->_header.append("\n");

	this->_header.append("Date: Mon, 27 Jul 2009 12:28:53 GMT\n\
	Server: huyaache/2.2.14 (Win32)\n\
	Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n\
	Content-Length: 420\n\
	Content-Type: text/html\n\
	Connection: Closed\n\n");
}