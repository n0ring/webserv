#pragma once

#include <poll.h>
#include <iostream>
#include <vector>
#include <map>
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
	private: 
		std::map<std::string, std::string> list;
	public: 
		Mime(void) {
			this->list["aac"] = "audio/aac";
			this->list["abw"] = "application/x-abiword";
			this->list["arc"] = "application/x-freearc";
			this->list["avi"] = "video/x-msvideo";
			this->list["azw"] = "application/vnd.amazon.ebook";
			this->list["bin"] = "application/octet-stream";
			this->list["bmp"] = "image/bmp";
			this->list["bz"] = 	"application/x-bzip";
			this->list["bz2"] = "application/x-bzip2";
			this->list["csh"] = "application/x-csh";
			this->list["css"] = "text/css";
			this->list["csv"] = "text/csv";
			this->list["doc"] = "application/msword";
			this->list["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
			this->list["eot"] = "application/vnd.ms-fontobject";
			this->list["epub"] = "application/epub+zip";
			this->list["gz"] = 	"application/gzip";
			this->list["gif"] = "image/gif";
			this->list["htm"] = "text/html";
			this->list["html"] = "text/html";
			this->list["ico"] = "image/vnd.microsoft.icon";
			this->list["ics"] = "text/calendar";
			this->list["jar"] = "application/java-archive";
			this->list["jpeg"] = "image/jpeg";
			this->list["jpg"] = "image/jpeg";
			this->list["js"] = 	"text/javascript";
			this->list["json"] = "application/json";
			this->list["jsonld"] = "application/ld+json";
			this->list["mid"] = "audio/midi";
			this->list["midi"] = "audio/midi";
			this->list["mjs"] = "text/javascript";
			this->list["mp3"] = "audio/mpeg";
			this->list["mpeg"] = "video/mpeg";
			this->list["mpkg"] = "application/vnd.apple.installer+xml";
			this->list["odp"] = "pplication/vnd.oasis.opendocument.presentation";
			this->list["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
			this->list["odt"] = "application/vnd.oasis.opendocument.text";
			this->list["oga"] = "audio/ogg";
			this->list["ogv"] = "video/ogg";
			this->list["ogx"] = "application/ogg";
			this->list["opus"] = "audio/opus";
			this->list["otf"] = "font/otf";
			this->list["png"] = "image/png";
			this->list["pdf"] = "application/pdf";
			this->list["php"] = "application/x-httpd-php";
			this->list["ppt"] = "application/vnd.ms-powerpoint";
			this->list["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
			this->list["rar"] = "application/vnd.rar";
			this->list["rtf"] = "application/rtf";
			this->list["sh"] = 	"application/x-sh";
			this->list["svg"] = "image/svg+xml";
			this->list["swf"] = "application/x-shockwave-flash";
			this->list["tar"] = "application/x-tar";
			this->list["tif"] = "image/tiff";
			this->list["tiff"] = "image/tiff";
			this->list["ts"] = 	"video/mp2t";
			this->list["ttf"] = "font/ttf";
			this->list["txt"] = "text/plain";
			this->list["vsd"] = "application/vnd.visio";
			this->list["wav"] = "audio/wav";
			this->list["weba"] = "audio/webm";
			this->list["webm"] = "video/webm";
			this->list["webp"] = "image/webp";
			this->list["woff"] = "font/woff";
			this->list["woff2"] = "font/woff2";
			this->list["xhtml"] = "application/xhtml+xml";
			this->list["xls"] = "application/vnd.ms-excel";
			this->list["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
			this->list["xml"] = "text/xml";
			this->list["xul"] = "application/vnd.mozilla.xul+xml";
			this->list["zip"] = "application/zip";
			this->list["3gp"] = "video/3gpp";
		}
		~Mime(void) {}

		std::string getMime(std::string &ext) {
			return this->list[ext];
		}
};



