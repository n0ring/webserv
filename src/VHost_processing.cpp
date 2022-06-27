#include "VHost.hpp"

// ver. 2 of route parsing
#include <unistd.h> // access
#define ROUTE_FIRST 1
int VHost::getListener(void) const {
	return this->_listener;
}

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
/*
	foo://example.com:8042/over/there?name=ferret#nose
	\_/   \______________/\_________/ \_________/ \__/
	 |           |            |            |        |
 scheme     authority       path        query   fragment
*/

void splitLastPath(routeParams& params) {
	if (params.path.empty()) {
		return ;
	} 
	size_t dotPos = params.path.back().find_last_of(".");
	
	if (dotPos == std::string::npos) {
		return ;
	}
	params.fileBaseName = params.path.back().substr(0, dotPos);
	params.ext = params.path.back().substr(dotPos + 1);
	params.path.erase(params.path.end() - 1);
}

void setParamObj(Request& request, routeParams& params) {
	std::vector<std::string>	routeArr;
	size_t posQueryChar, i;
	
	splitByChar(request.getParamByName("Route"), '/', routeArr);
	for (i = 0 ; i < routeArr.size(); i++) {
		posQueryChar = routeArr[i].find("?");
		if (posQueryChar == std::string::npos) {
			params.path.push_back(routeArr[i]);
		}
		else {
			params.path.push_back(routeArr[i].substr(0, posQueryChar));
			params.query = routeArr[i].substr(posQueryChar + 1);
			break;
		}
	}
	splitLastPath(params);
	i++;
	for (; i < routeArr.size(); i++) {
		params.pathRemainder.append("/" + routeArr[i]);
	}
}

void VHost::setRouteParamByDirSearch(routeParams& params, size_t i, VHost::locations_iter& it) {
	params.finalPathToFile.append(it->params["root"]);
	while (i < params.path.size()) {
		params.finalPathToFile.append("/" + params.path[i++]);
	}
	params.finalPathToFile.append("/");
	if (params.fileBaseName.empty()) {
		params.finalPathToFile.append(it->params["index"]);
	}
	else {
		params.finalPathToFile.append(params.fileBaseName + "." + params.ext);
	}
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

VHost::locations_iter	VHost::getLocation(routeParams& params) {
	std::vector<location>::iterator it;
	std::vector<location>::iterator ite = this->locations.end();
	std::string dirToFind;

	for (size_t i = 0; i < params.path.size(); i++) { // search by dirNames
		dirToFind.append("/" + params.path[i]);
		it =  findLocationMatch(this->locations, dirToFind); 
		if (it != ite) { // dir found set root + all dirs + filename 
			this->setRouteParamByDirSearch(params, i + 1, it);
			return it;
		}
	}
	it = findLocationMatch(this->locations, params.ext); // search for file ext
	if (it != ite) {
		params.finalPathToFile.append(it->params["root"] + "/" + params.fileBaseName + "." + params.ext);

	}
	else if (!params.fileBaseName.empty() + !params.ext.empty()) {
		std::string r = "/";
		it = findLocationMatch(this->locations, r); // serch for root (add file and try to open) !!!!!!
		if (it != ite) {
			params.finalPathToFile.append(it->params["root"] + "/");
		}
		params.finalPathToFile.append(params.fileBaseName + "." + params.ext);
	}
	return it;
}


void VHost::setLocation(Request& request, routeParams &paramObj, location **locToConnection) {
	VHost::locations_iter		currentLoc;
	std::string						fileToSend;
	setParamObj(request, paramObj);
	request.setQueryString(paramObj.query);
	currentLoc = this->getLocation(paramObj);
	if (currentLoc == this->locations.end()) {
		*locToConnection = NULL;
	} else {
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
