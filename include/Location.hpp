#pragma once 

class location {
	private:
		friend class VHost; 
		friend class Cgi; 
		typedef std::vector<std::string>::size_type size_type;
		
		std::vector<std::string>	names;
		std::string					root;
		std::vector<std::string>	methods; // ints
		std::string					autoindex;
		std::string					index;
		std::string					cgi;

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
				fileName = this->root + "/" + params[1] + "." + params[0];
			} 
			else { // only dir
				fileName = this->root + "/" + this->index;
			}
			if (fileName[0] == '/') fileName.erase(0, 1);
			return fileName;
		}

		bool isFormars() {
			return this->names.size() > 1;
		}

		bool isCgi() { return  !(this->cgi.empty()); }

		void toString() {
		std::cout << GREEN << "Location: { " << RESET << std::endl;
		std::cout << "Names: [ ";
		for (size_type i = 0; i < this->names.size(); i++) {
			std::cout << this->names[i] << " ";
		}
		std::cout << "]" << std::endl;
		std::cout << "Root: " << this->root << std::endl;
		std::cout << "Methods: [ ";
		for (size_type i = 0; i < this->methods.size(); i++) {
			std::cout << this->methods[i] << " ";
		}
		std::cout << "]" << std::endl;
		std::cout << "autoindex: " << autoindex << std::endl;
		std::cout << "index: " << index << std::endl;
		std::cout << GREEN << " } " << RESET << std::endl;
	}
};