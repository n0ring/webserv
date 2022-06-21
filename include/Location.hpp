#pragma once 

#include <vector>
#include <map>

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

	public:
		bool isLocationMatch(std::string &route) {
			std::vector<std::string>::iterator start = this->names.begin();
			std::vector<std::string>::iterator end = this->names.end();
			if ((*start).compare("*") == 0) {
				start++;
			}
			if (std::find(start, end, route) != end) {
				return true;
			}
			return false;
		}

		bool isMethodAllow(std::string& method) {
			std::vector<std::string>::iterator it;
			it = std::find(this->methods.begin(), this->methods.end(), method);
			return it != this->methods.end();
		}

		std::string getFileName(std::vector<std::string> &params) {
			std::string fileName;
			if (params.size() == 3) { // has file  root + dir 
				fileName = this->params["root"] + "/" + params[1] + "." + params[0];
			} 
			else { // only dir
				fileName = this->params["root"] + "/" + this->params["index"];
			}
			if (fileName[0] == '/') fileName.erase(0, 1);
			return fileName;
		}

		bool isFormars() {
			return this->names.size() > 1;
		}

		bool isCgi() { return  (this->params.count("cgi")); }

		bool hasErrorPage(void) { return !this->errorPages.empty();}
		std::string getErrorPage(int code) { 
			if (this->errorPages.count(code)) {
				return this->errorPages[code];
			}
			return "";
		 }


		void toString() {
			std::cout << GREEN << "Location: { " << RESET << std::endl;
			std::cout << "Names: [ ";
			for (size_type i = 0; i < this->names.size(); i++) {
				std::cout << this->names[i] << " ";
			}
			std::cout << "]" << std::endl;
			std::cout << "Root: " << this->params["root"] << std::endl;
			std::cout << "Methods: [ ";
			for (size_type i = 0; i < this->methods.size(); i++) {
				std::cout << this->methods[i] << " ";
			}
			std::cout << "]" << std::endl;
			std::cout << "autoindex: " << this->params["autoindex"] << std::endl;
			std::cout << "index: " << this->params["index"] << std::endl;
			std::cout << GREEN << " } " << RESET << std::endl;
	}
};