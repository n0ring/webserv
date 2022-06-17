
#include "VHost.hpp"

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

// 3 params - has file
// 1 param - only dir

// check for file ext - first loop
// check for dirs - sec loop if after first not found file ext


void setParamObj(std::vector<std::string>& v, routeParams& params) {
	size_t i = 0;
	for (; i < v.size(); i++) {
		if (v[i].find(".") != std::string::npos) {
			break;	
		}
		params.dirs.push_back(v[i]);
	}
	if (i == v.size()) {
		return ;
	}
	size_t baseNameEnd = v[i].find(".");
	if (baseNameEnd != std::string::npos && baseNameEnd != v[i].length() - 1) {
		params.fileBaseName = v[i].substr(0, baseNameEnd);
		size_t extEnd = baseNameEnd + 1;
		while (extEnd + 1 < v[i].size() && v[i][extEnd + 1] != '/' 
			&& v[i][extEnd + 1] != '?') extEnd++;
		params.ext = v[i].substr(baseNameEnd + 1, extEnd - baseNameEnd);
		params.afterFile.append(v[i].substr(extEnd + 1));
		i++;
		for (; i < v.size(); i++) {
			params.afterFile.append("/");
			params.afterFile.append(v[i]);
		}
	}
}

void VHost::setRouteParamByDirSearch(routeParams& params, size_t i, VHost::locations_iter& it) {
	params.finalPathToFile.append(it->root);
	while (i < params.dirs.size()) {
		params.finalPathToFile.append("/" + params.dirs[i++]);
	}
	params.finalPathToFile.append("/");
	if (params.fileBaseName.empty()) {
		params.finalPathToFile.append(it->index);
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

	for (size_t i = 0; i < params.dirs.size(); i++) { // search by dirNames
		dirToFind.append("/" + params.dirs[i]);
		it =  findLocationMatch(this->locations, dirToFind); 
		if (it != ite) { // dir found set root + all dirs + filename 
			this->setRouteParamByDirSearch(params, i + 1, it);
			return it;
		}
	}
	it = findLocationMatch(this->locations, params.ext); // search for file ext
	if (it != ite) {
		params.finalPathToFile.append(it->root + "/" + params.fileBaseName + "." + params.ext);
	}
	else if (!params.fileBaseName.empty() + !params.ext.empty()) {
		std::string r = "/";
		it = findLocationMatch(this->locations, r); // serch for root (add file and try to open) !!!!!!
		if (it != ite) {
			params.finalPathToFile.append(it->root + "/");
		}
		params.finalPathToFile.append(params.fileBaseName + "." + params.ext);
	}
	return it;
}



void GET(Request& request, routeParams &paramObj) {
	request.setFileNameToSend(paramObj.finalPathToFile);
}

void POST(Request& request, routeParams &paramObj) {
	(void) request;
	(void) paramObj;
}

void VHost::processHeader(Request& request, routeParams &paramObj) {
	VHost::locations_iter		currentLoc;
	VHost::locations_iter		currentLoc1;
	std::string					fileToSend;
	std::vector<std::string>	inputRouteParams;

	std::cout << "ROUTE IN PROCESS: " << request.getParamByName("Route") << std::endl;
	splitByChar(request.getParamByName("Route"), '/', inputRouteParams);
	setParamObj(inputRouteParams, paramObj);
	currentLoc = this->getLocation(paramObj);
	if (currentLoc == this->locations.end()) {
		request.setCurrentCode(404);
		std::cout << "location not found " << std::endl; 
		return ;
	}
	if (currentLoc->isMethodAllow(request.getParamByName("Method")) == false) {
		request.setCurrentCode(405);
		std::cout << "method not allowed " << std::endl; 
		return ;
	}
	if (currentLoc->isCgi()) {
		std::cout << "CGI" << std::endl;
		request.setCgiPid(Cgi::start(*currentLoc, TMP_FILE));
		request.setFileNameToSend(TMP_FILE);
	}
	else {
		std::string method = request.getParamByName("Method");
		if (!method.compare("GET")) {
			std::cout << "Method GET" << std::endl;
			GET(request, paramObj);
		}
		else if (!method.compare("POST")) {
			std::cout << "Method POST" << std::endl;
		}
		else if (!method.compare("DELETE")) {
			std::cout << "Method DELETE" << std::endl;
		}

	}
		request.setCurrentCode(200);
}

