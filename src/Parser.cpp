#include "Parser.hpp"

#define DEL_SERVER_WORD 6
#define SPLIT_SPACE " "
#define LOCATION_PARAM "location"


// check constacts for configServes
// check for errors in config (how parsers react)

ServerConfig createObj(std::string configText) { 
	size_t						startPos = 0;
	std::vector<std::string>	currentParams;
	ServerConfig				serverConfig;
	std::string					line;

	while (startPos < configText.length()) {
		line = getLine(configText, startPos);
		currentParams = sPPlit(line);
		if (currentParams.size() < 2) continue;
		if (currentParams.front().compare(LOCATION_PARAM)) {
			removeSemicolon(currentParams.back(), line);
			serverConfig.setServerParams(currentParams);
		} else {
			serverConfig.setNewLocation(currentParams);
			while (true) {
				line = getLine(configText, startPos);
				currentParams = sPPlit(line);
				if (currentParams.back().compare("}") == 0) break ;
				removeSemicolon(currentParams.back(), line);
				serverConfig.setLocationParam(currentParams);
			}
		}
	}
	serverConfig.toString();
	return serverConfig;
}

void Parser::parseConfig(std::vector<ServerConfig> &configsObjs, std::string configName) {
	std::vector<std::string>			configsVector;
	std::vector<std::string>::iterator	it, ite;
	std::string							configStr;
	ServerConfig						configObj;

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
		configObj = createObj(*it);
		configObj.validate();
		configsObjs.push_back(configObj);
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

