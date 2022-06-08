#pragma once 

#include <iostream>
#include "utils.hpp" // sPPlit getline

#define END_OF_HEADER "\r\n\r\n"
#define END_OF_HEADER_SHIFT 4
class Request {
	private:
		std::string _method;
		std::string	_route; // (location)
		std::string	_ip;

		std::string	_header;
		std::string _fileToSend;
		int			_fileToSave;

		int			_currentCode;

	
	public:
		Request(void) {
			this->_fileToSave = -1;
			this->_currentCode = 0;
		};
		~Request(void) {};
		Request(Request const &other);
		Request &operator=(Request const &other);


		std::string &getRoute(void);

		std::string& getHeader(void);
		void	setHeader(std::string& buf_in);
		void	parseHeader();
		void resetObj(void);

		std::string& getFileToSend() { return this->_fileToSend; }
		void setFileToSend(std::string fileName) { 
			this->_fileToSend = fileName;
		}
		void setCurrentCode(int code) { this->_currentCode = code; }
		int getCurrentCode() { return  this->_currentCode; }
		std::string& getMethod(void) { return this->_method; }
};