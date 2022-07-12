#include "VHost.hpp"

// ver. 3 of route parsing (full path. start to search and remove last parts)
// ver. 3.1 change logic for cgi location. root is root. cgi is full path to exec file

int VHost::getListener(void) const {
	return this->_listener;
}

void setQueryString(routeParams& params) {
	size_t queryStringStart = params.fullRoute.find("?");
	if (queryStringStart == std::string::npos) {
		return ;
	}
	params.query =  params.fullRoute.substr(queryStringStart + 1);
	params.fullRoute.erase(queryStringStart);
}

void checkForFileExt(routeParams& params) {
	size_t fileExtStart = params.fullRoute.find_last_of(".");
	size_t fileExtEnd;
	if (fileExtStart == std::string::npos) {
		return ;
	}
	fileExtEnd = ++fileExtStart;
	while (fileExtEnd < params.fullRoute.length() 
		&& params.fullRoute[fileExtEnd] != '/') {
		fileExtEnd++;
	}
	params.ext = params.fullRoute.substr(fileExtStart, fileExtEnd - fileExtStart);
}

void setParamObj(Request& request, routeParams& params) {
	params.fullRoute = request.getParamByName("Route");
	setQueryString(params);
	checkForFileExt(params);
}

VHost::locations_iter findLocationMatch(std::vector<location>& locations, std::string s) {
	std::vector<location>::iterator it = locations.begin();
	std::vector<location>::iterator ite = locations.end();
	
	if (s.empty())  {
		return ite;
	}
	for (; it != ite; it++) { 
		if (it->isLocationMatch(s)) break;
	}
	return it;
}

void removeLastDir(routeParams& params) {
	size_t lastDirStart = params.fullRoute.find_last_of("/");
	if (lastDirStart == std::string::npos) {
		params.fullRoute.clear();
		return ;
	}
	params.pathStack.push(params.fullRoute.substr(lastDirStart));
	params.fullRoute.erase(lastDirStart);
}

void setFinalPathToFile(VHost::locations_iter it, routeParams& params) {
	std::string indexFile = it->getParamByName("index");
	params.finalPathToFile.append(it->getParamByName("root"));
	while (!params.pathStack.empty()) {
		params.finalPathToFile.append(params.pathStack.top());
		params.pathInfo.append(params.pathStack.top());
		params.pathStack.pop();
	}
	if (params.ext.empty() && !indexFile.empty()) { // if no file add index file from loc
		params.finalPathToFile.append("/");
		params.finalPathToFile.append(it->getParamByName("index"));
	}
	std::cout << RED << "final route path: " << params.finalPathToFile << RESET << std::endl;
}

VHost::locations_iter	VHost::getLocation(routeParams& params) {
	std::vector<location>::iterator it;
	std::vector<location>::iterator ite = this->locations.end();

	while (!params.fullRoute.empty()) { // by dirs
		it =  findLocationMatch(this->locations, params.fullRoute);
		if (it != ite) { // found
			setFinalPathToFile(it, params);
			return it;
		}
		removeLastDir(params);
	}
	it = findLocationMatch(this->locations, params.ext);
	if (it == ite) {
		it = findLocationMatch(this->locations, "/");
	}
	if (params.ext.empty()) {
		return ite;
	}
	setFinalPathToFile(it, params);
	return it;
}


void VHost::setLocation(Request& request, routeParams &paramObj, location **locToConnection) {
	VHost::locations_iter			currentLoc;
	std::string						fileToSend;

	bzero(&paramObj, sizeof(paramObj));
	setParamObj(request, paramObj);
	// request.setQueryString(paramObj.query);
	
	currentLoc = this->getLocation(paramObj);
	if (currentLoc == this->locations.end()) {
		*locToConnection = NULL;
	} else {
		std::string root = currentLoc->getParamByName("root");
		request.setHeaderParam("QueryString", paramObj.query);
		request.setHeaderParam("PathInfo", paramObj.pathInfo);
		request.setHeaderParam("Root", root);
		*locToConnection = &(*currentLoc);
	}
}

VHost*	VHost::changeVhost(std::string& hostName) {
	for (size_t i = 0; i < this->vHostsWithSamePort.size(); i++) {
		if (vHostsWithSamePort[i].getServerName() == hostName) {
			return &(vHostsWithSamePort[i]);
		}
	}
	return NULL;
}

std::string VHost::getErrorPage(int code) {
	if (this->errorPages.count(code)) {
		return this->errorPages[code];
	}
	return "";
}
