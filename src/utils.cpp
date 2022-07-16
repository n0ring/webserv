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

void truncStr(std::string &s) {
	if (s[0] == '/') {
		s.erase(0, 1);
	}
	if (s[s.length() - 1] == '/') {
		s.erase(s.length() - 1);
	}
}

std::string Utils::getDefaultErrorPage(int code) {
	std::string page = "\n";
	std::string message = "This is the default page. User didn't provide it. Error: ";
	page.append("\
	<!DOCTYPE html>\
		<html>\
			<head>\
				<meta charset=\'utf-8\'>\
				<meta http-equiv=\'X-UA-Compatible' content=\'IE=edge\'>\
				<title>Error!</title>\
				<meta name=\'viewport\' content=\'width=device-width, initial-scale=1\'>\
			</head>\
		<body style=\"background: black;\">\
			<h1 style=\"margin: 8rem auto; width: fit-content; padding: 3rem; font-size: 2.05rem; border-radius: 15px; border: 1px dashed; color: #f7eedf; font-family: monospace; text-align: center;\">");
	page.append(message);
	page.append(std::to_string(code) + " " + this->getResponceName(code));
	page.append("</h1>\
	</body>\
	</html>");
	return page;
}


Mime::Mime(void) {
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


HttpResponceNames::HttpResponceNames(void) {
	this->list[200] = "OK";
	this->list[201] = "Created";
	this->list[202] = "Accepted";
	this->list[204] = "No Content";
	this->list[300] = "Multiple Choice";
	this->list[301] = "Moved Permanently";
	this->list[302] = "Found";
	this->list[303] = "See Other";
	this->list[304] = "Not Modified";
	this->list[305] = "Use Proxy";
	this->list[306] = "Switch Proxy";
	this->list[307] = "Temporary Redirect";
	this->list[308] = "Permanent Redirect";
	this->list[400] = "Bad Request";
	this->list[403] = "Forbidden";
	this->list[404] = "Not Found";
	this->list[405] = "Method Not Allowed";
	this->list[408] = "Request Timeout";
	this->list[411] = "Length Required";
	this->list[413] = "Request Entity Too Large";
	this->list[500] = "Internal Server Error";
	this->list[501] = "Not Implemented";
	this->list[502] = "Bad Gateway";
	this->list[503] = "Service Unavailable";
	this->list[504] = "Gateway Timeout";
	this->list[505] = "HTTP Version Not Supported";
}

std::string HttpResponceNames::getName(int code) {
	std::string name = this->list[code];
	return name;
}

std::string Mime::getMimeByExt(std::string &ext) {
	return this->list[ext];
}

std::string Utils::getMime(std::string& ext) {
	return this->mime.getMimeByExt(ext);
}

std::string Utils::getResponceName(int code) {
	return this->responceNames.getName(code);
}

void	exitWithMsg(std::string msg) {
	std::cerr << msg << std::endl;
	exit(-1);
}


