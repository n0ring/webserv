#include "Cgi.hpp"

void	Cgi::findPathToApp(std::string& pathToApp, std::string& fileToExec) {
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

void Cgi::setEnv(char **env) {
	// env[1] = (char *) std::string("PATH_INFO=").append(getenv("PATH_INFO")).c_str();
	env[0] = (char *) std::string("PATH=").append(getenv("PATH")).c_str();
	env[1] = NULL;
}



void child(const char *tmpFile, std::vector<char *>& argv, char* env[]) {
	int					ofd;
	
	remove(tmpFile);
	// dup2(inputFile, STDIN_FILENO); 
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


int	Cgi::start(location &loc, const char *tmpFile, Request& request) { // return exit status
	std::string			pathToApp;
	int					pid;
	std::vector<char *>	argv;
	char*				env[2];
	std::string			fileToExec = loc.params["root"] + "/" + loc.params["cgi"];

	(void) request;
	if (loc.params.count("bin")) {
		pathToApp.append("/" + loc.params["bin"]);
	} else {
		findPathToApp(pathToApp, fileToExec);
	}
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
		child(tmpFile, argv, env);
	}
	else {
		waitpid(pid, 0, 0);
	}
	return pid;
}