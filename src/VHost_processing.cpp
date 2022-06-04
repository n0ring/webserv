#include "VHost.hpp"

#define ROUTE_FIRST 1
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

// /index/dir/some

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
		endPos = startPos > ROUTE_FIRST ? --startPos : startPos; // one dir or not
	}
	params.push_back(route.substr(0, endPos));
}

// 3 params - has file
// 1 param - only dir

location& VHost::getLocation(std::vector<std::string>& params) {
	std::vector<location>::iterator it = this->locations.begin();
	std::vector<location>::iterator ite = this->locations.end();
	std::string route = params.size() == 3 ? params[0].substr(1) : params[0];
	for (; it != ite; it++) {
		if (it->isLocationMatch(route)) {
			break;
		}
	}
	if (it == ite) {
		return this->locations[0];
	}
	return *it;
}

void VHost::handleRequest(Request& request, Responce& responce) {
	std::vector<std::string>	routeParams;
	std::string					body;

	setRouteParams(request.getRoute(), routeParams);

	for (int i = 0; i < (int) routeParams.size(); i++) {
		std::cout << "params " << i << " " << routeParams[i] << std::endl;
	}
	if (routeParams.empty()) {
		std::cout << RED << "This shouldn't have happened. Call your admin (or mom) and run. And God help us\n" << RESET;
		return ;
	}
	location currentRoute = this->getLocation(routeParams);
	currentRoute.toString();
	std::cout << "fileName for search: " << currentRoute.getFileName(routeParams) << std::endl;
	// getlocation
	// valid location
	// findFile
	// convertFileToString(fileName, body);


	std::string fileName = currentRoute.getFileName(routeParams).erase(0, 1);
	convertFileToString(fileName, body);
	responce.setHeader(g_head);
	responce.setBody(body);
}
