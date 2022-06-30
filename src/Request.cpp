#include "Request.hpp"


Request::Request(Request const & other) : _currentCode(0), _cgiPid(other._cgiPid) {
	}


Request & Request::operator=(Request const &other) {
	if (this != &other) {
		
	}
	return *this;
}


std::string& Request::getParamByName(std::string paramName) {
	return this->_headerParams[paramName];
}

void Request::resetObj(void) {

	this->_header.clear();
	this->_headerParams.clear();
	this->_fileToSend.clear();
	this->_fileToSave= -1;
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
	std::cout << BLUE << "-----------header in--------------------" << std::endl;
	std::cout << this->_header << std::endl;
	std::cout << "-----------end of header-------------" <<  RESET << std::endl;
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

void Request::setQueryString(std::string& str) {
	if (!str.empty())
	this->_headerParams["QueryString"] = str;
}