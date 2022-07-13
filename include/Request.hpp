#pragma once 

#include <iostream>
#include <map>
#include "utils.hpp" // sPPlit getline

#define END_OF_HEADER "\r\n\r\n"
#define END_OF_HEADER_SHIFT 4
#define SUPPORTED_PROTOCOL "HTTP/1.1"
class Request {
	private:
		std::string							_header;
		std::map<std::string, std::string>	_headerParams;
		std::string							_fileToSend;
		int									_currentCode;

		Request &operator=(Request const &other);
		Request (Request const &other);	
	public:
		Request (void);
		~Request(void);

		void			setHeader(std::string& buf_in);
		std::string&	getHeader(void);
		void			parseHeader();
		void			resetObj(void);
		std::string&	getFileToSend();
		void			setFileNameToSend(std::string fileName);
		void			setCurrentCode(int code);
		int				getCurrentCode();
		std::string&	getParamByName(std::string paramName);
		void			setHeaderParam(std::string param, std::string& val);
};