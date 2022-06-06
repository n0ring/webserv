#pragma once

#include <iostream> // string io
#include <fstream> // ifstream

#define BUFFER 2048

class Responce {
	private:
		std::string		_header;
		std::string		_body;
		std::ifstream	ifs;
		size_t			fileLen;
		size_t			headerSended;
		size_t			bodySended;

	public:
		Responce(void) : headerSended(0), bodySended(0) {};
		~Responce(void) {};


		void		setHeader(std::string header);

		bool prepareFileToSend(const  char *fileName);
		
		size_t getFileSize();
		size_t getHeaderSize();

		size_t	fillBuffer(char *buf);
		void	closeFile();
};