#include "Parser.hpp"

#define DEL_SERVER_WORD 6
#define SPLIT_SPACE " "
#define LOCATION_PARAM "location"


// check constacts for configServes
// check for errors in config (how parsers react)
VHost createObj(std::string configText) { 
	size_t						startPos = 0;
	std::vector<std::string>	currentParams;
	VHost						vHost;
	std::string					line;

	while (startPos < configText.length()) {
		line = getLine(configText, startPos);
		currentParams = sPPlit(line);
		if (currentParams.size() < 2) continue;
		if (currentParams.front().compare(LOCATION_PARAM)) {
			removeSemicolon(currentParams.back(), line);
			vHost.setServerParams(currentParams);
		} else {
			vHost.setNewLocation(currentParams);
			while (true) {
				line = getLine(configText, startPos);
				currentParams = sPPlit(line);
				if (currentParams.back().compare("}") == 0) break ;
				removeSemicolon(currentParams.back(), line);
				vHost.setLocationParam(currentParams);
			}
		}
	}
	// vHost.toString();
	return vHost;
}

bool	checkForMatchIpPort(std::vector<VHost> &vHosts, VHost& newVhost) {
	int			portCurrent, portForCheck  = newVhost.getPort();
	std::string	serverNameForCheck = newVhost.getServerName();
	std::string	hostCurrent, hostForCheck = newVhost.getHost();

	for (unsigned long i = 0; i < vHosts.size(); i++) {
		portCurrent = vHosts[i].getPort();
		hostCurrent = vHosts[i].getHost();
		if (( hostCurrent == hostForCheck || hostCurrent == "0.0.0.0"
				|| hostForCheck == "0.0.0.0") 
				&& portCurrent == portForCheck) {
			if (serverNameForCheck == vHosts[i].getServerName()) {
				std::cerr << "Same serverName: " << serverNameForCheck 
				<< " for host " << hostCurrent << " on port: " << portCurrent
				<< std::endl;
				exit(-1);
			}
			vHosts[i].addHostSamePort(newVhost);
			return true;
		}
	}
	return false;
}

void Parser::parseConfig(std::vector<VHost> &vHosts, std::string configName) {
	std::vector<std::string>			configsVector;
	std::vector<std::string>::iterator	it, ite;
	std::string							configStr;
	VHost								vHostObj;

	convertFileToString(configName, configStr);
	deleteComments(configStr);
	splitConfigToServers(configStr, configsVector);
	
	if (configsVector.size() < 1) {
		std::cerr << "no configs in config file: " << configName << std::endl;
		exit(-1);
	}

	it = configsVector.begin();
	ite = configsVector.end();
	for (; it != ite; it++) {
		vHostObj = createObj(*it);
		// check for match 
		vHostObj.validate();
		if (checkForMatchIpPort(vHosts, vHostObj)) {
			
		} else {
			vHosts.push_back(vHostObj);
		}
	}
}

void Parser::splitConfigToServers(std::string config, std::vector<std::string>& configsVector) {
	std::stack<char>			st;
	
	for (unsigned long i = 0; i < config.length();)
	{
		unsigned long start = config.find("server ", i);
		if (start == std::string::npos) {
			break ;
		}
		else {
			start += DEL_SERVER_WORD;
		}
		while (start < config.length() && config[start] != '{') {
			start++;
		}
		int k = start;
		while (k < (int) config.length()) {
			if (config[k] == '{') {
				st.push('{');
			}
			else if (config[k] == '}' && st.top() == '{') {
				st.pop();
			}
			k++;
			if (st.empty()) {
				break;
			}
		}
		configsVector.push_back(config.substr(start, k - start));
		i = k;
	}
}

