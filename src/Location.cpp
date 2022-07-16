#include "Location.hpp"


location::location(location const & other) {
	*this = other;
}


location& location::operator=(location const & other) {
	if (this != &other) {
		this->names = other.names;
		this->methods = other.methods;
		this->params = other.params;
		this->errorPages = other.errorPages;
		this->redirectCode = other.redirectCode;
	}
	return *this;
}

std::string	location::getParamByName(std::string param) {
	if (this->params.count(param)) {
		return this->params[param];
	}
	return "";
}

void 	location::validate(void) {
	if (this->methods.empty())
		exitWithMsg("Allowed methods not found in location");
	if (this->names.empty())
		exitWithMsg("Names in location not found");
	if (this->names[0] != "*" && this->names[0][0] != '/')
		exitWithMsg("Dir name has start with \"/\": " + this->names[0]);
	for (size_t i = 0; i < this->methods.size(); i++) {
		if (this->methods[i] != "GET"
			&& this->methods[i] != "POST"
			&& this->methods[i] != "DELETE") {
		exitWithMsg("Incorrect method: " + this->methods[i]);
		}
	}
}

bool location::isLocationMatch(std::string &route) {
	std::vector<std::string>::iterator start = this->names.begin();
	std::vector<std::string>::iterator end = this->names.end();
	if ((*start).compare("*") == 0) {
		start++;
	}
	if (std::find(start, end, route) != end) {
		return true;
	}
	return false;
}

bool location::isCgi() { return  (this->params.count("cgi")); }
bool location::hasErrorPage(void) { return !this->errorPages.empty();}


bool location::isMethodAllow(std::string& method) {
	std::vector<std::string>::iterator it;
	it = std::find(this->methods.begin(), this->methods.end(), method);
	return it != this->methods.end();
}

std::string location::getErrorPage(int code) { 
	if (this->errorPages.count(code)) {
		return this->errorPages[code];
	}
	return "";
}

int location::getRedirectCode(void) { return this->redirectCode; }

std::string location::getLocationName() {
	if (names.empty()) {
		return "";
	}
	return this->names[0];
}

void location::toString() {
	std::cout << GREEN << "Location: { " << RESET << std::endl;
	std::cout << "Names: [ ";
	for (size_type i = 0; i < this->names.size(); i++) {
		std::cout << this->names[i] << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "Root: " << this->params["root"] << std::endl;
	std::cout << "Methods: [ ";
	for (size_type i = 0; i < this->methods.size(); i++) {
		std::cout << this->methods[i] << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "autoindex: " << this->params["autoindex"] << std::endl;
	std::cout << "index: " << this->params["index"] << std::endl;
	std::cout << GREEN << " } " << RESET << std::endl;
}

bool	location::isMaxBodyExceeded(int bodyLen) {
	long int			maxBody;
	std::string	maxBodyStr = this->getParamByName("max_client_body_size");

	if (maxBodyStr.empty()) {
		return false;
	}
	stringToNum(maxBodyStr, maxBody);
	return bodyLen > maxBody;
}
