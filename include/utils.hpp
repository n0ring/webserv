#pragma once

#include <poll.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

pollfd make_fd(int fd, int event); // inline? 


void convertFileToString(std::string &configName, std::string &config);
void deleteComments(std::string &config);
void removeSemicolon(std::string& s, std::string& line);
std::vector<std::string> sPPlit(std::string s);
std::string getLine(std::string& str, size_t& startPos);
void truncStr(std::string &s);
std::pair<std::string, std::string> splitInPair(std::string &line);
void splitByChar(std::string &s, char delimiter, std::vector<std::string>& v);
std::string getDefaultErrorPage(int code);

template<typename T>
void stringToNum(std::string &s, T& num) {
	if (s.empty()) {
		num = 0;
		return ;
	}
	std::stringstream ss;
	ss << s;
	ss >> num;
}

// make map? 
class Mime {
	public: 
		static void set(std::string &ext, std::string& mime) {
			if (!ext.compare("html")) {
				mime = "text/html";
				return ;
			}
			if (!ext.compare("ico")) {
				mime = "image/x-icon";
				return ;
			}
			if (!ext.compare("png")) {
				mime = "image/png";
				return ;
			}
			if (!ext.compare("jpg") || !ext.compare("jpeg")) {
				mime = "image/jpeg";
				return ;
			}
			mime.clear();
		}
};