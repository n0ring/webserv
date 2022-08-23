#pragma once 

#include <vector>
#include <map>
#include "utils.hpp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

class location {
	private:
		friend class VHost; 
		friend class Cgi; 
		typedef std::vector<std::string>::size_type size_type;
		
		std::vector<std::string>			names;
		std::vector<std::string>			methods; // ints
		std::map<std::string, std::string>	params;
		std::map<int, std::string>			errorPages;
		int									redirectCode;
	public:
		location(void) {}
		location(location const & other);
		location& operator=(location const & other);

		std::string	getParamByName(std::string param);
		bool isLocationMatch(std::string &route);

		bool isMethodAllow(std::string& method);
		bool isCgi();
		bool hasErrorPage(void);
		std::string getErrorPage(int code);
		int getRedirectCode(void);
		std::string getLocationName();
		void validate(void);
		void toString();
		bool	isMaxBodyExceeded(int bodyLen);
};