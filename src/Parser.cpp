#include "Parser.hpp"

#define DEL_SERVER_WORD 6

// open file 
// split text into vector strs by Server {...}
// "server" + { 
// until } for first

// setup map for every config
// 

Parser::Parser(std::string configName) : _configName(configName),
		_ifs(configName, std::ifstream::in) { }


ServerConfig createObj(std::string configText) { 
	(void) configText;
	return  ServerConfig(8080, "127.0.0.1", 10);
}


void Parser::createServerConfigs(std::vector<ServerConfig> &configsObjs) {
	std::vector<std::string>			configsVector;
	std::vector<std::string>::iterator	it, ite;
	std::string							config;


	if (this->_ifs.is_open() == false) {
		std::cerr << "incorrect file name: " << this->_configName << std::endl;
		exit(-1);
	}
	config.assign((std::istreambuf_iterator<char>(this->_ifs)),
						std::istreambuf_iterator<char>());
	this->_ifs.close();
	splitConfigToServers(config, configsVector);
	
	if (configsVector.size() < 1) {
		std::cerr << "no configs in config file: " << this->_configName << std::endl;
		exit(-1);
	}
	it = configsVector.begin();

	ite = configsVector.end();
	// for (; it != ite; it++) {
	// 	// std::cout << *it << std::endl;
	// 	configsObjs.push_back(createObj(*it));
	// }

	configsObjs.push_back(createObj(*it));

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