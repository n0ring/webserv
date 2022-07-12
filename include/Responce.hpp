#pragma once

#include <iostream> // string io
#include <fstream> // ifstream
#include "Location.hpp"
#include "Header.hpp"
#include "utils.hpp"
#define BUFFER 100


class Responce {
	private:
		std::string		_header;
		std::ifstream	ifs;
		size_t			fileLen;
		size_t			headerSended;
		size_t			bodySended;
		int				code;
		// int				contentLength;
		// std::string		contentType;
		std::string		MIME;
		std::string		fileExtToSend;
		Header			headerObj;
		Mime			mimeList;

		Responce(Responce const &other);
		Responce& operator=(Responce const &other);
	public:
		Responce(void);
		~Responce(void);

		void		setHeader(std::string header);
		bool		prepareFileToSend(std::string fileName, std::string& bodyOut);
		void		setCode(int c);
		size_t		getFileSize();
		size_t		getHeaderSize();
		size_t		fillBuffer(char *buf, std::string& bodyOut);
		void		resetObj();
		void		createHeader(location* loc);
		std::string	getCgiHeader(void);
		void		createCGiHeader(void);
		void		setCgiHeaderToResponce(std::string& cgiHeader);
		void		setParamToHeader(std::string param);
		void		closeOutputFile(void);
};

