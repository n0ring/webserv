#include "VHost.hpp"

#include <unistd.h> // access
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
// check for dirs - sec loop if after first not found file ext
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

void findPathToApp(std::string& pathToApp, std::string& fileToExec) {
	std::ifstream	ifs;
	char			c;
	std::string		line;

	ifs.open(fileToExec);
	if (!ifs.is_open()) {
		std::cout << "cgi file not open" << std::endl;
		return ;
	}
	while (!ifs.eof()) {
		c = ifs.get();
		if (c == '\n') {
			if (line.find("#!") == std::string::npos) {
				line.clear();
				continue;
			}
			break;
		}
		else {
			line.push_back(c);
		}
	}
	if (line.find("#!") != std::string::npos) {
		pathToApp = line.substr(line.find("/"));
	}
}

void setEnv(char **env) {
	// env[1] = (char *) std::string("PATH_INFO=").append(getenv("PATH_INFO")).c_str();
	env[0] = (char *) std::string("PATH=").append(getenv("PATH")).c_str();
	env[1] = NULL;
}

int	VHost::cgiStart(location &loc) { // return name file to read and send
	int					ofd;
	std::string			pathToApp;
	int					pid;
	std::vector<char *>	argv;
	char*				env[2];
	std::string			fileToExec = loc.root + "/" + loc.cgi;

	findPathToApp(pathToApp, fileToExec);
	if (access(pathToApp.c_str(), X_OK) == 0) {
		argv.push_back((char *) pathToApp.c_str());
	}
	argv.push_back((char *) fileToExec.c_str());
	argv.push_back(NULL);
	setEnv(env);
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}
	if (pid == 0)  {
		ofd = open(TMP_FILE, O_RDWR | O_CREAT | O_TRUNC, 777);
		if (ofd == -1) {
			perror("open tmp file");
			exit(1);
		}
		dup2(ofd, STDOUT_FILENO);
		if (execve(argv[0], &(argv[0]), env)  == -1) {
			close(ofd);
			perror("execve");
			exit(1);
		}
	}
	else {
		waitpid(pid, 0, 0);
	}
	return pid;
}

void VHost::processHeader(Request& request) {
	std::vector<std::string>	inputRouteParams;
	VHost::locations_iter		currentLoc;
	std::string					fileToSend;

	setRouteParams(request.getParamByName("Route"), inputRouteParams);
	currentLoc =  this->getLocation(inputRouteParams);
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
		request.setCgiPid(this->cgiStart(*currentLoc));
		request.setFileNameToSend(TMP_FILE);
	}
	else {
		request.setFileNameToSend(currentLoc->getFileName(inputRouteParams));
	}
	request.setCurrentCode(200);
	// set actions for every method? 
}
