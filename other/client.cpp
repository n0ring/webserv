#include <sys/socket.h>

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <vector>
#include <atomic>
#include <thread>
#include <sys/poll.h>

#define PORT 8080

// req: /index.html HTTP/1.1



void setSockAddr_in(struct sockaddr_in *address) {
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = inet_addr("127.0.0.1");
	// address->sin_addr.s_addr = htons(INADDR_ANY);
	address->sin_port = htons(PORT);
}

void check(int val, std::string process) {
	if (val < 0) {
		perror(process.c_str());
		exit(-1);
	}
}

void readMsg(int fd) {
	while (true) {
		char buf[1024];
		if (recv(fd, buf, 1024, 0) <= 0) {
			std::cout << "connect closed" << std::endl;
			exit(0);
		}
		std::cout << buf;
		bzero(buf, 1024);
	}
}

void writeMsg(int fd) {
	std::string			str;
	while (true) {
		// std::cout << "you: ";
		std::getline(std::cin, str);
		check(send(fd, str.c_str(), str.length(), 0), "send");
	}
}

void startConnection(void) {
	struct sockaddr_in	address;
	char				req[1024];
	int					addrlen		= sizeof(address);
	std::vector<std::thread> threads;

	// int yes=1;
	// check(
	// 	setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)), "setsockopt");
	
	setSockAddr_in(&address);
	std::string suff;

	int client_fd = socket(PF_INET, SOCK_STREAM, 0);


	check(client_fd, "socket");
	check(connect(client_fd, (struct sockaddr *) &address, (socklen_t ) addrlen), "connect");
		std::cout << "connect success" << std::endl;
	threads.push_back(std::thread(readMsg, client_fd));
	threads.push_back(std::thread(writeMsg, client_fd));
	threads[0].join();
	threads[0].detach();
}

int main(void) {

	startConnection();
	return 0;
}

