
#pragma once
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <vector>
#include <arpa/inet.h>  // inet_addr
#include <algorithm>
#include "utils.hpp" // parserUtils deleteComment, sPPlit, getLine
#include "Request.hpp"
#include "Responce.hpp"
#define LOCATION_PARAM "location"
#define TMP_FILE "tmp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */


struct routeParams { // no / on start and end
	std::vector<std::string>	dirs; // before file
	std::string					fileBaseName;
	std::string					ext;
	std::string					afterFile;
	std::string					finalPathToFile;
};
class location {
	private:
		friend class VHost; 
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

class VHost {
	private:
		int			_port; // form cfg
		std::string	_ip; // from cfg
		std::string	_serverName; // from cfg
		int 		_backlog; // from cfg ?? 
		int 		_maxBody; // from cfg

		int			_listener;
		sockaddr	_address; 
		socklen_t	_addrlen;
		std::vector<location> locations;
		
		void	setupSocket(void);
		void	setupSockAddr_in(void);

	public:
		typedef std::vector<location>::iterator locations_iter;
		
		VHost(void);
		VHost(VHost const &other);
		VHost(int port, std::string ip, int bl);
		VHost &operator=(VHost const &other);
		~VHost(void);

		// setup before start
		void	setNewLocation(std::vector<std::string> params);
		void	setLocationParam(std::vector<std::string> params);
		void	setServerParams(std::vector<std::string> params);
		void	validate();
		int		setup(void);

		// procccccesssssss..
		int			getListener(void) const;
		int			acceptNewConnection();
		void		setResponce(Request& request, Responce &responce);

		void	processHeader(Request& request, routeParams& routeObj);
		int		cgiStart(location &loc);
		locations_iter  getLocation(routeParams& params);

		// delete this
		void toString();
};