#pragma once

#include "iostream" // string
#include "Location.hpp" 
#include "VHost.hpp"

class Cgi {	
	private: 
		static void findPathToApp(std::string& pathToApp, std::string& fileToExec);
		static void setEnv(char **env);
	public:

		static	int	start(location &loc, const char *tmpFile, Request& request);
};