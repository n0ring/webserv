#pragma once 

#include <iostream>
#include "utils.hpp" // sPPlit getline

class Request {
	private:
		std::string _method;
		std::string	_route; // (location)
		std::string	_ip;
	
	public:
		Request(void) {};
		~Request(void) {};
		Request(Request const &other);
		Request &operator=(Request const &other);

		void parseStr(std::string reqStr);

		std::string &getRoute(void);
		void resetObj(void);
};