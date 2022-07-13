#pragma once

#include <poll.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#define BUFFER 2048

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#define CODE_OK 200
#define CODE_CREATED 201
#define CODE_ACCEPTED 202
#define CODE_NO_CONTENT 204
#define CODE_MULTIPLE_CHOICE 300
#define CODE_MOVED_PERMANENTLY 301
#define CODE_FOUND 302
#define CODE_SEE_OTHER 303
#define CODE_NOT_MODIFIED 304
#define CODE_USE_PROXY 305
#define CODE_SWITCH_PROXY 306
#define CODE_TEMPORARY_REDIRECT 307
#define CODE_PERMANENT_REDIRECT 308
#define CODE_BAD_REQUEST 400
#define CODE_FORBIDDEN 403
#define CODE_NOT_FOUND 404
#define CODE_METHOD_NOT_ALLOWED 405
#define CODE_REQUEST_TIMEOUT 408
#define CODE_LENGTH_REQUIRED 411
#define CODE_REQUEST_ENTITY_TOO_LARGE 413
#define CODE_INTERNAL_SERVER_ERROR 500
#define CODE_NOT_IMPLEMENTED 501
#define CODE_BAD_GATEWAY 502
#define CODE_SERVICE_UNAVAILABLE 503
#define CODE_GATEWAY_TIMEOUT 504
#define CODE_HTTP_VERSION_NOT_SUPPORTED 505

pollfd make_fd(int fd, int event); // inline? 


void convertFileToString(std::string &configName, std::string &config);
void deleteComments(std::string &config);
void removeSemicolon(std::string& s, std::string& line);
std::vector<std::string> sPPlit(std::string s);
std::string getLine(std::string& str, size_t& startPos);
void truncStr(std::string &s);
std::pair<std::string, std::string> splitInPair(std::string &line);
void splitByChar(std::string &s, char delimiter, std::vector<std::string>& v);
void	exitWithMsg(std::string msg);

template<typename T>
void stringToNum(std::string &s, T& num, bool isHex = false) {
	if (s.empty()) {
		num = 0;
		return ;
	}
	std::stringstream ss;
	if (isHex) {
		ss << std::hex << s;
	} else {
		ss << s;
	}
	ss >> num;
}

class Mime {
	private: 
		std::map<std::string, std::string> list;
	public: 
		Mime(void);
		~Mime(void) {}
		std::string getMimeByExt(std::string &ext);
};

class HttpResponceNames {
	private: 
		std::map<int, std::string> list;
	public:
	HttpResponceNames(void);
	~HttpResponceNames(void) {}
	std::string getName(int code);
};

class Utils {
	Mime				mime;
	HttpResponceNames	responceNames;
	
	public:
		std::string getDefaultErrorPage(int code);
		std::string getMime(std::string& ext);
		std::string getResponceName(int code);
		
};
