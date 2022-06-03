#include "VHost.hpp"


std::string g_head = "HTTP/1.1 200 OK\n\
	Date: Mon, 27 Jul 2009 12:28:53 GMT\n\
	Server: huyaache/2.2.14 (Win32)\n\
	Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n\
	Content-Length: 88\n\
	Content-Type: text/html\n\
	Connection: Closed\n\n";

int VHost::getListener(void) const {
	return this->_listener;
}

void setRouteParams(std::string route, std::vector<std::string>& params) {
	int startPos = route.length() - 1;
	int endPos = startPos + 1;
	while (startPos >= 0) {
		if (route[startPos] == '/' || route[startPos] == '.') {
			break ;
		}
		startPos--;
	}
	if (startPos < 0) { return ; }
	if (route[startPos] == '.') { // if filename in route
		params.push_back(route.substr(startPos));
		endPos = startPos;
		while (startPos >= 0) {
			if (route[startPos] == '/') {
				startPos++;
				break ;
			}
			startPos--;
		}
		if (startPos < 0) { return ;}
		params.push_back(route.substr(startPos, endPos - startPos));
		endPos = startPos - 1;
	}
	params.push_back(route.substr(0, endPos));
}


location& VHost::getLocation(std::string route) {
	(void) route;

	return this->locations[0];
}

void VHost::handleRequest(Request& request, Responce& responce) {
	std::vector<std::string>	routeParams;
	std::string					body;


	setRouteParams(request.getRoute(), routeParams); // after last /
	for (int i = 0; i < (int) routeParams.size(); i++) {
		std::cout << "params " << i << " " << routeParams[i] << std::endl;
	}


	location currentRoute = this->getLocation(request.getRoute());

	// getlocation
	// valid location
	// findFile
	// convertFileToString(fileName, body);


	std::string fileName = "index.html"; // mock
	convertFileToString(fileName, body);
	responce.setHeader(g_head);
	responce.setBody(body);
}
