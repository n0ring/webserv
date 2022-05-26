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
#define SOK 1000

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

void readMsg(std::vector<int>& v) {
	int n = 100;
	char buf[1024];
	for (int i = 0; i < 100; i++) {
		char buf[1024];
		if (recv(v[i], buf, 1024, 0) <= 0) {
			std::cout << "connect closed" << std::endl;
			exit(0);
		}
		std::cout << buf;
		bzero(buf, 1024);
	}
}

void writeMsg(std::vector<int>& v) {
	std::string			str;
	for (int i = 0; i < 100; i++) {
		send(v[i], "hello", 5, 0);
	}
	// while (true) {
	// 	// std::cout << "you: ";
	// 	std::getline(std::cin, str);
	// 	check(send(fd, str.c_str(), str.length(), 0), "send");
	// }
}



void startConnection(void) {
	struct sockaddr_in	address;
	char				req[1024];
	int					addrlen		= sizeof(address);
	std::vector<std::thread> threads;
	std::vector<int>	sockets;
	// int yes=1;
	// check(
	// 	setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)), "setsockopt");
	
	setSockAddr_in(&address);
	std::string suff;

	//  int client_fd = socket(PF_INET, SOCK_STREAM, 0);
	for (int i = 0; i < SOK; i++) {
		sockets.push_back(socket(PF_INET, SOCK_STREAM, 0));
	}

	for (int i  = 0; i < SOK; i++) {
		connect(sockets[i], (struct sockaddr *) &address, (socklen_t ) addrlen);
	}
	// check(client_fd, "socket");
	// check(connect(client_fd, (struct sockaddr *) &address, (socklen_t ) addrlen), "connect");
	// 	std::cout << "connect success" << std::endl;
	// threads.push_back(std::thread(readMsg, sockets));
	// threads.push_back(std::thread(writeMsg, sockets));
	// threads[0].join();
	// threads[0].detach();

	for (int i = 0; i < SOK; i++) {
		for (int k = 0; k < 1000; k++) {
			send(sockets[i], "hello", 5, 0);
		}
	}
		for (int i = 0; i < 100; i++) {
			close(sockets[i]);
		}
}

int main(void) {

	startConnection();
	return 0;
}

