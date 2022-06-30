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

void setEnv(std::vector<std::string>& envVector, Request& request) {
	char *pwd = getenv("PWD");
	envVector.push_back("REQUEST_METHOD=" + request.getParamByName("Method"));
	envVector.push_back("CONTENT_LENGTH=" + request.getParamByName("Content-Length"));
	envVector.push_back("CONTENT_TYPE=" + request.getParamByName("Content-Type"));
	envVector.push_back("REQUEST_METHOD=" + request.getParamByName("Method"));
	envVector.push_back("QUERY_STRING=" + request.getParamByName("QueryString"));

	if (pwd) {
		envVector.push_back("PATH_INFO=" + std::string(pwd)); // change after
	}
}

void child(std::string& tmpInputFile, std::string& tmpOutputFile,
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

int	Cgi::start(location &loc, std::string& tmpInputFile, std::string& tmpOutputFile, Request& request) { // return exit status
	std::string			pathToApp;
	int					pid, status;
	std::vector<char *>	argv;
	std::string			fileToExec = loc.params["root"] + "/" + loc.params["cgi"];

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

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}
	if (pid == 0)  {
		child(tmpInputFile, tmpOutputFile, argv, request);
	}
	else {
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
	}
	return EXIT_SUCCESS;
}

void Cgi::preprocessCgi(Connection& connect) {
	int fd = -1;
	if (connect.getCurrectCode() >= 400) {
		return ;
	}
	// set env jere
	remove(connect.getCgiInputFileName().c_str());
	fd = open(connect.getCgiInputFileName().c_str(), O_RDWR | O_CREAT | O_TRUNC);
	if (fd == -1) {
		perror("create input file fail");
		connect.setCurrentCode(500);
	}
	else {
		std::cout << "input file was created." << std::endl;
		connect.setFileInputFd(fd);
		connect.sendBodyToFile();
	}
}
