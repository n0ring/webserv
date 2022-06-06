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

void Responce::closeFile() {this->ifs.close();}
