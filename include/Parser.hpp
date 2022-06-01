#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include "ServerConfig.hpp"


// change names to public names
// parseConfig
// parseRequest

class Parser {

	public:
		Parser();
		~Parser(void) {}
		void parseConfig(std::vector<ServerConfig> &configsObjs, std::string configName);
		void splitConfigToServers(std::string config, std::vector<std::string>& confs);
		void splitParamsByName(std::string config, std::vector<std::string>& configsVector, std::string paramName);


};