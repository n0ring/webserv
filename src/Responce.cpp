#include "Responce.hpp"

void Responce::setHeader(std::string header) {
	this->_header = header;
}

void Responce::setBody(std::string body) {
	this->_body = body;
}


std::string  Responce::getBody(void) { return this->_body; }
std::string Responce::getHeader(void) { return this->_header; }