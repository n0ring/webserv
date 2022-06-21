#pragma once

#include <iostream> // string io
#include <fstream> // ifstream
#include <sys/stat.h> // stat (check is file regular)
#include "Location.hpp"
#include "utils.hpp"
#define BUFFER 700


class Responce {
	private:
		std::string		_header;
		std::string		_body;
		std::ifstream	ifs;
		size_t			fileLen;
		size_t			headerSended;
		size_t			bodySended;

		int				code;
		int				contentLength;
		std::string		contentType;
		std::string		MIME;
		std::string		fileExtToSend;


	public:
		Responce(void) : headerSended(0), bodySended(0) {};
		~Responce(void) {};


		void		setHeader(std::string header);
		
		bool prepareFileToSend(std::string fileName);
		
		void setCode(int c);

		size_t getFileSize();
		size_t getHeaderSize();

		size_t	fillBuffer(char *buf);
		void	resetObj();


		void	createHeader(location* loc);
};

