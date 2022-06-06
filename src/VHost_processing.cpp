#include "VHost.hpp"

#define ROUTE_FIRST 1
std::string g_head = "HTTP/1.1 200 OK\n\
	Date: Mon, 27 Jul 2009 12:28:53 GMT\n\
	Server: huyaache/2.2.14 (Win32)\n\
	Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n\
	Content-Length: 420\n\
	Content-Type: text/html\n\
	Connection: Closed\n\n";

int VHost::getListener(void) const {
	return this->_listener;
}

// /index/dir/some

void setRouteParams(std::string& route, std::vector<std::string>& params) {
	size_t extStart, fStart;
	extStart = route.find_last_of(".");
	fStart = route.find_last_of("/");
	if (extStart != std::string::npos) {
		fStart++;
		params.push_back(route.substr(extStart + 1));
		params.push_back(route.substr(fStart, extStart - fStart));
		if (fStart > 0) fStart--;
		params.push_back(route.substr(0, fStart));
	}
	else {
		params.push_back(route);
	}
}

// 3 params - has file
// 1 param - only dir

// check for file ext - first loop
// check for dirs - sec loop if after first end
VHost::locations_iter  VHost::getLocation(std::vector<std::string>& routeParams) {
	std::vector<location>::iterator it = this->locations.begin();
	std::vector<location>::iterator ite = this->locations.end();

	for (; it != ite; it++) { 
		if (it->isLocationMatch(routeParams[0])) {
			break;
		}
	}
	if (it != ite) {
		return it;
	}
	if (routeParams.size() == 3) { // check for dirs if before for files
		it = this->locations.begin();
		for (; it != ite; it++) {
			if (it->isLocationMatch(routeParams[2])) {
				break;
			}
		}
	}
	return it;
}



void VHost::handleRequest(Request& request, Responce& responce) {
	std::vector<std::string>	routeParams;
	std::string					body;
	VHost::locations_iter		currentLoc;
	std::string					fileToSend;

	setRouteParams(request.getRoute(), routeParams);

	for (int i = 0; i < (int) routeParams.size(); i++) {
		std::cout << "params " << i << " " << routeParams[i] << std::endl;
	}
	if (routeParams.empty()) {
		std::cout << RED << "This shouldn't have happened. Call your admin (or mom) and run. And God help us\n" << RESET;
		return ;
	}
	
	currentLoc =  this->getLocation(routeParams);
	// validate request (location, method, file)
	
	if (currentLoc == this->locations.end()) {
		fileToSend.append("www/errors/404.html");
	} else {
		fileToSend = currentLoc->getFileName(routeParams);
		currentLoc->toString();
	}
	
	std::cout << "file To send: " << fileToSend << std::endl;
	// getlocation
	// valid location (method)
	// findFile


	// setResponce
	// openfile//
	if (!responce.prepareFileToSend(fileToSend.c_str())) {
		if (responce.prepareFileToSend("www/errors/404.html") == false)  {
			exit(1);
		}
	}
	responce.setHeader(g_head);
	// responce.setBody(body);
}
