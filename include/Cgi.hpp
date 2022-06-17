#pragma once

#include "iostream" // string
#include "Location.hpp" 
#include "VHost.hpp"

class Cgi {	
	static void findPathToApp(std::string& pathToApp, std::string& fileToExec) {
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

	static void setEnv(char **env) {
		// env[1] = (char *) std::string("PATH_INFO=").append(getenv("PATH_INFO")).c_str();
		env[0] = (char *) std::string("PATH=").append(getenv("PATH")).c_str();
		env[1] = NULL;
	}

	public:

		static	int	start(location &loc, const char *tmpFile) { // return name file to read and send
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
				ofd = open(tmpFile, O_RDWR | O_CREAT | O_TRUNC, 777);
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
};