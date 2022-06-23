
#pragma once
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <vector>
#include <map>
#include <arpa/inet.h>  // inet_addr
#include <algorithm>
#include "utils.hpp" // parserUtils deleteComment, sPPlit, getLine
#include "Request.hpp"
#include "Responce.hpp"
#include "Location.hpp"
// #include "Cgi.hpp"

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
	std::vector<std::string>	path; // before file
	std::string					fileBaseName;
	std::string					ext;
	std::string					query;
	std::string					finalPathToFile;
	std::string					pathRemainder;
};
class VHost {
	private:
		int			_port; // form cfg
		std::string	_ip; // from cfg
		std::string	_serverName; // from cfg
		int 		_backlog; // from cfg ?? 
		int 		_maxBody; // from cfg
		std::map<int, std::string>	errorPages;

		int			_listener;
		sockaddr	_address; 
		socklen_t	_addrlen;
		std::vector<location>	locations;
		std::vector<VHost>		vHostsWithSamePort;
		
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
		int			getPort(void) const { return this->_port; }
		std::string	getServerName(void) const {return this->_serverName; }
		std::string	getHost(void) const {return this->_ip; }
		void	addHostSamePort(VHost newHost) {
			this->vHostsWithSamePort.push_back(newHost);
		}

		// procccccesssssss..
		int			getListener(void) const;
		int			acceptNewConnection();
		void		setResponce(Request& request, Responce &responce);
		std::string	getErrorPage(int code);
		void	processHeader(Request& request, routeParams& routeObj, location **currentLoc);
		VHost*	changeVhost(std::string& hostName);
		locations_iter 	getLocation(routeParams& params);
		void			setRouteParamByDirSearch(routeParams& params, size_t i, VHost::locations_iter& it);

		// delete this
		void toString();
};