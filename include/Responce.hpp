#pragma once

#include <iostream> // string io

class Responce {
	private:
		std::string _header;
		std::string	_body;
		// fd to send

	public:
		Responce(void) {};
		~Responce(void) {};

		void		setHeader(std::string header);
		void		setBody(std::string body);
		std::string	getBody(void);
		std::string	getHeader(void);
};