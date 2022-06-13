#pragma once 

#include <iostream>
#include <map>
#include "utils.hpp" // sPPlit getline

#define END_OF_HEADER "\r\n\r\n"
#define END_OF_HEADER_SHIFT 4
#define SUPPORTED_PROTOCOL "HTTP/1.1"
class Request {
	private:
		std::string	_ip;

		std::string	_header;
		std::map<std::string, std::string> _headerParams;

		std::string _fileToSend;
		int			_fileToSave;

		int			_currentCode;
		int			_cgiPid;
	
	public:
		Request(void) {
			this->_fileToSave = -1;
			this->_currentCode = 0;
			this->_cgiPid = -1;
		};
		~Request(void) {};
		Request(Request const &other);
		Request &operator=(Request const &other);

		std::string& getHeader(void);
		void	setHeader(std::string& buf_in);
		void	parseHeader();
		void	resetObj(void);

		std::string& getFileToSend() { return this->_fileToSend; }
		void setFileNameToSend(std::string fileName) { 
			this->_fileToSend = fileName;
		}
		void setCurrentCode(int code) { this->_currentCode = code; }
		int getCurrentCode() { return  this->_currentCode; }

		std::string& getParamByName(std::string paramName);
		int				getCgiPid(void) {return this->_cgiPid; }
		void setCgiPid(int pid) { this->_cgiPid = pid; }
};