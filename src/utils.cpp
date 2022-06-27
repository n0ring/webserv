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
		exit(1);
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

void splitByChar(std::string &s, char delimiter, std::vector<std::string>& v) {
	size_t start = 0, end;
	while (start < s.length()) {
		while (start < s.length() && s[start] == delimiter ) start++;
		end = start;
		while (end < s.length() && s[end] != delimiter) end++;
		v.push_back(s.substr(start, end - start));
		start = end + 1;
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

std::pair<std::string, std::string> splitInPair(std::string &line) {
	size_t divider = line.find(" ");
	std::string first, second;
	if (divider == std::string::npos) {
		return std::make_pair("", "");
	}
	first = line.substr(0, divider);
	second = line.substr(divider + 1);
	if (first[first.length() - 1] == ':') {
		first.erase(first.end() - 1);
	}
	if (second[second.length() - 1] == '\n') {
		second.erase(second.end() - 1);
	}
	return std::make_pair(first, second);
}

std::string getLine(std::string& str, size_t& startPos) {
	size_t		endPos	=  std::min(str.find('\n', startPos), str.find('\r', startPos));

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

bool isRecieveOver(std::string req) { // need to refactor or delete
	int last = req.length() - 1;
	if (req.length() < 4) {
		return false;
	}
	if (req[last] == 10 && req[last - 1] == 13
	 	&& req[last - 2] == 10 && req[last - 3] == 13) {
		return true;
	}
	return false;
}

void truncStr(std::string &s) {
	if (s[0] == '/') {
		s.erase(0, 1);
	}
	if (s[s.length() - 1] == '/') {
		s.erase(s.length() - 1);
	}
}

std::string getDefaultErrorPage(int code) {
	std::string page;
	std::string message = "This is default page. User don't provide it. Error: ";
	page.append("\
	<!DOCTYPE html>\
<html>\
<head>\
	<meta charset='utf-8'>\
	<meta http-equiv='X-UA-Compatible' content='IE=edge'>\
	<title>Page Title</title>\
	<meta name='viewport' content='width=device-width, initial-scale=1'>\
</head>\
<body style=\"background: black;\">\
	<h1 style=\"color: blueviolet;  margin: auto; width: 50%\">");
	
	page.append(message);
	page.append(std::to_string(code));
	page.append("</h1>\
	</body>\
	</html>");
	return page;
}