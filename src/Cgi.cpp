#include "Cgi.hpp"

void setEnv(std::vector<std::string>& envVector, Request& request) {
	char *pwd = getenv("PWD");
	envVector.push_back("REQUEST_METHOD=" + request.getParamByName("Method"));
	envVector.push_back("CONTENT_LENGTH=" + request.getParamByName("Content-Length"));
	envVector.push_back("CONTENT_TYPE=" + request.getParamByName("Content-Type"));
	envVector.push_back("REQUEST_METHOD=" + request.getParamByName("Method"));
	envVector.push_back("QUERY_STRING=" + request.getParamByName("QueryString"));
	envVector.push_back("PATH_INFO=" + request.getParamByName("PathInfo"));
	if (pwd) {
		envVector.push_back("PATH_TRANSLATED=" + std::string(pwd) + "/"
			+ request.getParamByName("Root") + request.getParamByName("PathInfo")); 
	}
}

void child(const std::string& tmpInputFile, const std::string& tmpOutputFile,
			std::vector<char *>& argv,  Request& request) {
	int	ofd, ifd;
	std::vector<std::string> envVector;
	setEnv(envVector, request);

	char* env[envVector.size() + 1];
	for (size_t i = 0; i < envVector.size(); i++) {
		env[i] = (char *) envVector[i].c_str();
	}
	env[envVector.size()] = NULL;
	remove(tmpOutputFile.c_str());
	ofd = open(tmpOutputFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 777);
	ifd = open(tmpInputFile.c_str(), O_RDONLY);
	if (ofd == -1) {
		perror("CGI:open tmp output");
		std::cerr << RED << tmpOutputFile.c_str() << RESET << std::endl;
		exit(1);
	}
	if (ifd == -1) {
		perror("CGI:open tmp input");
		std::cerr << RED << tmpInputFile.c_str() << RESET << std::endl;
		exit(-1);
	}
	dup2(ifd, STDIN_FILENO); 
	dup2(ofd, STDOUT_FILENO);
	if (execve(argv[0], &(argv[0]), env)  == -1) {
		close(ofd);
		perror("execve");
		exit(1);
	}
}

int	Cgi::start(location &loc, const std::string& tmpInputFile, const std::string& tmpOutputFile, Request& request) { // return exit status
	std::string			pathToApp;
	int					pid, status;
	std::vector<char *>	argv;
	std::string			fileToExec = loc.params["cgi"];
	int 				waitExec = 1000;


	if (loc.params.count("bin")) {
		pathToApp.append("/" + loc.params["bin"]);
	}
	if (access(pathToApp.c_str(), X_OK) == 0) {
		argv.push_back((char *) pathToApp.c_str());
	}
	argv.push_back((char *) fileToExec.c_str());
	argv.push_back(NULL);

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}
	if (pid == 0)  {
		child(tmpInputFile, tmpOutputFile, argv, request);
	}
	else {
		while (waitpid(pid, &status, WNOHANG) == 0 && waitExec) {
			usleep(1000);
			waitExec--;
		}
		if (waitExec == 0) {
			kill(pid, SIGKILL);
			return TIMEOUT_CGI;
		}

		if (WIFEXITED(status))
			return WEXITSTATUS(status);
	}
	return EXIT_SUCCESS;
}
