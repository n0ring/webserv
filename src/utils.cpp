#include "utils.hpp"

pollfd make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;
	return newfd;
}


				/* --->parser utils<--- */
void convertFileToString(std::string &configName, std::string &config) {
	std::ifstream ifs(configName);
	if (ifs.is_open() == false) {
		std::cerr << "incorrect file name: " << configName << std::endl;
		exit(-1);
	}
	config.assign((std::istreambuf_iterator<char>(ifs)),
						std::istreambuf_iterator<char>());
	ifs.close();
}

void deleteComments(std::string &config) {
	for (size_t i = 0; i < config.length(); i++) {
		if (config[i] == '#') {
			size_t endOfComment = config.find('\n', i);
			if (endOfComment == std::string::npos) {
				endOfComment = config.length();
			}
			config.erase(i, endOfComment - i);
		}
	}
}

std::vector<std::string> sPPlit(std::string s) {
	std::vector<std::string>	res; 
	size_t						start = 0, end;

	while (start < s.length()) {
		while (start < s.length() && isspace(s[start])) start++;
		end = start;
		while (end < s.length() && !isspace(s[end])) end++;
		res.push_back(s.substr(start, end - start));
		start = end + 1;
	}

	return res;
}

std::string getLine(std::string& str, size_t& startPos) {
	size_t		endPos	= str.find('\n', startPos);
	std::string	line;
	if (endPos == std::string::npos) {
		startPos = str.length();
		return "";
	}
	line = str.substr(startPos, endPos - startPos);
	startPos = endPos + 1;
	return line;
}

void removeSemicolon(std::string& s, std::string& line) {
	int last = s.length();
	if (last == 0) return ;
	if (s[last - 1] == ';') {
		s.erase(s.end() - 1);
	}
	else {
		std::cerr << "Line in config should end with \";\" : " << line << std::endl;
		exit(-1);
	}
}