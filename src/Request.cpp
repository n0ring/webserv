#include "Request.hpp"


Request::Request(Request const & other) : _method(other._method),
			_route(other._route), _ip(other._ip), _currentCode(0) { }


Request & Request::operator=(Request const &other) {
	if (this != &other) {
		this->_method = other._method;
		this->_ip = other._ip;
		this->_route = other._route;
	}
	return *this;
}


std::string & Request::getRoute(void)  { return this->_route; }


void Request::resetObj(void) {
	this->_method.clear();
	this->_route.clear();
	this->_ip.clear();
	this->_header.clear();
	this->_fileToSave= -1;
	this->_currentCode = 0;
}

std::string& Request::getHeader(void) { return this->_header; }

void Request::setHeader(std::string& buf_in) {
	size_t endHeader = buf_in.find(END_OF_HEADER);
	if (endHeader != std::string::npos) {
		this->_header = buf_in.substr(0, endHeader + 1);
		this->parseHeader();
		buf_in.erase(0, endHeader + END_OF_HEADER_SHIFT);
	}
}

void Request::parseHeader() {
	std::string					line;
	size_t						start = 0;
	std::vector<std::string>	params;

	params = sPPlit(getLine(this->_header, start));
	this->_method = params[0];
	this->_route = params[1];
	std::cout << "request: " << this->_method << " " << this->_route << std::endl;
}



// GET / HTTP/1.1 
// Host: localhost:8080
// Connection: keep-alive
// sec-ch-ua: " Not A;Brand";v="99", "Chromium";v="98", "Google Chrome";v="98"
// sec-ch-ua-mobile: ?0
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.102 Safari/537.36
// sec-ch-ua-platform: "macOS"
// Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
// Sec-Fetch-Site: same-origin
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Dest: image
// Referer: http://localhost:8080/
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9
// Cookie: myCookie=snickerdoodle