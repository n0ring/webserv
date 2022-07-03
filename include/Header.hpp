#pragma once

#include "iostream"
#include "utils.hpp"

class Header {
	private:
		std::string status;
		std::string contentType;
		std::string contentLength;
		std::string connectionStatus;
		std::string unknownParams;

		std::string result;
		std::string end;

	public:
		Header(void);
		Header(Header const& other);
		Header& operator=(Header const& other);
		~Header(void);

		void		reset(void);
		std::string	getHeaderStr(void);
		void		setStatus(std::string nstatus);
		void		setContentType(std::string nContentType);
		void		setContentLength(std::string nContentLenth);
		void		setConnectionStatus(std::string nConnectionStatus);
		void		setEndOfHeader(bool isNeed);
		void		checkCgiHeader(std::string& cgiHeader, bool& isCgiHeaderValid);
		bool		isParamMatch(std::vector<std::string>& currentParams, std::string line);
		bool		validateParams(void);

};