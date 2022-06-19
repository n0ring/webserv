#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include "VHost.hpp"


// change names to public names
// parseConfig
// parseRequest

class Parser {

	public:
		Parser() {};
		~Parser(void) {}
		void parseConfig(std::vector<VHost> &configsObjs,
							std::string configName);
		void splitConfigToServers(std::string ,std::vector<std::string>& );

};