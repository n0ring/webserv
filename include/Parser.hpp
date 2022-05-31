#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include "ServerConfig.hpp"


// change names to public names
// parseConfig
// parseRequest

class Parser {
	private:
		std::string		_configName;
		std::ifstream	_ifs;


	public:
		Parser(std::string);
		~Parser(void) {}
		void createServerConfigs(std::vector<ServerConfig> &configsObjs);
		void splitConfigToServers(std::string config, std::vector<std::string>& confs);


};