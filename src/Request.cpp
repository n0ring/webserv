#include "Request.hpp"


Request::Request(Request const & other) : _ip(other._ip),
	_currentCode(0), _cgiPid(other._cgiPid) { }


Request & Request::operator=(Request const &other) {
	if (this != &other) {
		this->_ip = other._ip;
	}
	return *this;
}


std::string& Request::getParamByName(std::string paramName) {
	return this->_headerParams[paramName];
}

void Request::resetObj(void) {
	this->_ip.clear();
	this->_header.clear();
	this->_headerParams.clear();
	this->_fileToSave= -1;
	this->_fileToSend.clear();
	this->_currentCode = 0;
	this->_cgiPid = -1;
}

std::string& Request::getHeader(void) { return this->_header; }

void Request::setHeader(std::string& buf_in) {
	size_t endOfHeaderShift = 4;
	if (this->_header.empty() == false) {
		return ;
	}
	size_t endHeader = buf_in.find(END_OF_HEADER);
	if (endHeader == std::string::npos) {
		endHeader = buf_in.find("\n\n");
		endOfHeaderShift = 2;
	}
	if (endHeader != std::string::npos) {
		this->_header = buf_in.substr(0, endHeader + 1);
		this->parseHeader();
		buf_in.erase(0, endHeader + endOfHeaderShift);
	}
	std::cout << "-----------header--------------------" << std::endl;
	std::cout << this->_header << std::endl;
	std::cout << "-----------end of header-------------" << std::endl;
}

void Request::parseHeader() {
	std::string					line;
	size_t						start = 0;
	std::vector<std::string>	params;

	params = sPPlit(getLine(this->_header, start));
	if (params.size() < 3) {
		this->_currentCode = 505;
		return ;
	}
	this->_headerParams.insert(std::make_pair("Method", params[0]));
	this->_headerParams.insert(std::make_pair("Route", params[1]));
	if (params[2].compare(SUPPORTED_PROTOCOL)) {
		this->_currentCode = 505;
		return ;
	}
	while (start < this->_header.length()) {
		line = getLine(this->_header, start);
		this->_headerParams.insert(splitInPair(line));
	}
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