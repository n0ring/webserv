#include "Parser.hpp"

#define DEL_SERVER_WORD 6
#define SPLIT_SPACE " "

// open file 
// split text into vector strs by Server {...}
// "server" + { 
// until } for first

// setup vectors for every config
// transform to obj


// getParamName
// setparam (paramName, value); inner logic for setting and choose param

Parser::Parser() { }


std::string getLine(std::string& str, size_t& startPos) {
	size_t		endPos	= str.find('\n', startPos);
	std::string	line;
	if (endPos == std::string::npos) {
		startPos = str.length();
		return "";
	}
	line = str.substr(startPos, endPos - startPos);
	startPos = endPos + 1;
	return line;
}

ServerConfig createObj(std::string configText) { 
	size_t startPos = 0;

	while (startPos < configText.length()) {
		std::string line = getLine(configText, startPos);
		std::cout << "line: " << line << std::endl;
	}


	return  ServerConfig(80, "0.0.0.0", 10);
}



void deleteComments(std::string &config) {
	for (size_t i = 0; i < config.length(); i++) {
		if (config[i] == '#') {
			size_t endOfComment = config.find('\n', i);
			if (endOfComment == std::string::npos) {
				endOfComment = config.length();
			}
			config.erase(i, endOfComment - i);
		}
	}
}

void convertFileToString(std::string &configName, std::string &config) {
	std::ifstream ifs(configName);
	if (ifs.is_open() == false) {
		std::cerr << "incorrect file name: " << configName << std::endl;
		exit(-1);
	}
	config.assign((std::istreambuf_iterator<char>(ifs)),
						std::istreambuf_iterator<char>());
	ifs.close();
}

void Parser::parseConfig(std::vector<ServerConfig> &configsObjs, std::string configName) {
	std::vector<std::string>			configsVector;
	std::vector<std::string>::iterator	it, ite;
	std::string							configStr;

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
		configsObjs.push_back(createObj(*it));
	}

}

void Parser::splitParamsByName(std::string config, std::vector<std::string>& configsVector,
						std::string paramName) {
	std::stack<char>			st;
	
	for (unsigned long i = 0; i < config.length();)
	{
		unsigned long start = config.find(paramName + SPLIT_SPACE, i);
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


// server {}