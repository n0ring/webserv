#pragma once

#include "iostream" // string
// #include "Location.hpp" 
// #include "VHost.hpp"
#include "Connection.hpp"

class Connection;

class Cgi {	
	private: 
		static void findPathToApp(std::string& pathToApp, std::string& fileToExec);
;
	public:
		static	void preprocessCgi(Connection& connect);	
		static	int	start(location &loc, const std::string& tmpInputFile,
				const std::string& tmpOutputFile, Request& request);
};