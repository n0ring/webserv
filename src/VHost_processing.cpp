#include "VHost.hpp"

#define ROUTE_FIRST 1


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

void VHost::processHeader(Request& request) {
	std::vector<std::string>	routeParams;
	VHost::locations_iter		currentLoc;

	setRouteParams(request.getRoute(), routeParams);
	currentLoc =  this->getLocation(routeParams);
	
	if (currentLoc == this->locations.end()) {
		request.setCurrentCode(404);
		std::cout << "location not found " << std::endl; 
		return ;
	}
	if (currentLoc->isMethodAllow(request.getMethod()) == false) {
		request.setCurrentCode(405);
		std::cout << "method not allowed " << std::endl; 
		return ;
	}
	request.setFileToSend(currentLoc->getFileName(routeParams));

	request.setCurrentCode(200);
}


void VHost::setResponce(Request& request, Responce& responce) {
	std::string					body;
	std::string					fileToSend;

	if (request.getCurrentCode() >= 400) {
		request.setFileToSend("www/errors/404.html");
	}
	if (!responce.prepareFileToSend(request.getFileToSend().c_str())) {
		responce.setCode(404);
		return ;
		if (responce.prepareFileToSend("www/errors/404.html") == false)  {
			exit(1);
		}
	}
	std::cout << "file To send: " << request.getFileToSend()  << std::endl;
	responce.setCode(request.getCurrentCode());
}
