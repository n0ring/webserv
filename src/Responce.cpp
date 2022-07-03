#include "Responce.hpp"

void Responce::setHeader(std::string header) {
	this->_header = header;
}

bool Responce::prepareFileToSend(std::string fileName) {
	if (this->ifs.is_open()) {
		this->ifs.close();
	}
	this->ifs.open(fileName, std::ifstream::in);
	if (this->ifs.is_open() == false) {
		return false;
	}
	this->ifs.seekg (0, ifs.end);
	this->fileLen = this->ifs.tellg();
	ifs.seekg(0, ifs.beg);
	this->fileExtToSend = fileName.substr(fileName.find_last_of(".") + 1);
	std::cout << fileName << " filelen: " << this->fileLen << std::endl;
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

	// std::cout << "file pos in fill buffer: " << this->ifs.tellg() << std::endl;

	if (this->headerSended < this->getHeaderSize()) {
		this->headerSended += shiftHead = this->_header.copy(buf, BUFFER);
	}
	if (this->ifs.is_open() && shiftHead < BUFFER && this->bodySended < this->fileLen) {
		this->ifs.read(buf + shiftHead, BUFFER - shiftHead);
		this->bodySended = this->ifs.eof() ? this->fileLen : (size_t) this->ifs.tellg(); 
	}
	// std::cout << "----buffer to send----" << std::endl;
	// std::string tmp;
	// tmp.append(buf);
	// std::cout << tmp << std::endl;
	// std::cout << "----------------------" << std::endl;
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
	this->headerObj.reset();
}

void Responce::setCode(int c) {
	this->code = c;
}

std::string Responce::getCgiHeader(void) {
	std::string cgiHeader;
	char buf[BUFFER];
	if (!this->ifs.is_open()) {
		return "";
	}
	while (this->ifs.rdstate() != std::ifstream::failbit) {
		bzero(buf, BUFFER);
		this->ifs.getline(buf, BUFFER);
		if (this->ifs.rdstate() & std::ifstream::failbit || buf[0] == '\0') {
			break;
		}
		cgiHeader.append(buf);
		cgiHeader.append("\n");
	}
	// ifs.seekg(0, ifs.beg);
	// count file size
	std::cout << "file pos: " << this->ifs.tellg() << std::endl;
	return cgiHeader;
}

void Responce::createHeader(location* loc) {
	(void) loc;

	Mime::set(this->fileExtToSend, this->MIME);
	this->headerObj.setStatus("HTTP/1.1 " + std::to_string(this->code) + " OK");
	this->headerObj.setContentType("Content-Type: " + this->MIME);
	this->headerObj.setContentLength("Content-Length: " + std::to_string(this->fileLen));
	this->headerObj.setConnectionStatus("Connection: Close");
	this->headerObj.setEndOfHeader(true);



	this->_header = this->headerObj.getHeaderStr();
	std::cout << GREEN << "-----header to send------" << std::endl;
	std::cout << this->_header << RESET << std::endl;
}

void	Responce::setCgiHeaderToResponce(std::string& cgiHeader, bool& isCgiHeaderValid) {
	if (cgiHeader.empty()) {
		return ;
	}
	this->headerObj.checkCgiHeader(cgiHeader, isCgiHeaderValid);
}

