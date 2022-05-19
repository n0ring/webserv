#include <sys/socket.h>

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <fcntl.h>

#include <poll.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#define PORT 8080

// req: /index.html HTTP/1.1

std::string processReq(std::string req) {
	std::string s(req);
	int start = 0, end = 0;

	while (s[start] != '/') {start++; }
	start++;
	end = start;
	while (s[end] != ' ') { end++; }
	s = s.substr(start, end - start);
	return s;
}

std::string getFileContent(std::string fileName) {
	std::ifstream ifs(fileName);
	std::stringstream buffer;

	buffer << ifs.rdbuf();
	ifs.close();
	return buffer.str();
}

std::string createResponse(std::string req) {
	std::string html = getFileContent(processReq(req));
	std::string responce;
	responce.append("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:");  //" 25\n\n<h1>Hello world!!!</h1>")
	int len = responce.length();
	len += html.length() + 5;
	responce.append( std::to_string(len));
	responce.append("\n\n");
	responce.append(html);
	// std::cout << responce << std::endl;
	return responce;
}

void setSockAddr_in(sockaddr_in *address) {
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

void sendBuf(int fd, const char *buf, int len) {
	int rev = 0;
	int sended = 0;
	while (sended < len) {
		rev = send(fd, buf + sended, len - sended, 0);
		check(rev, "send");
		sended += rev;
	}
}

void selectLogic(int server_fd, sockaddr_in *address, int addrlen) {
	int		newSocket;
	int		fdmax = server_fd;
	fd_set		tmp_set, set, setWrite;
	
	FD_ZERO(&set);
	FD_ZERO(&setWrite);
	FD_SET(server_fd, &set);
	while (true) {
		tmp_set = set;
		check(   select(fdmax + 1, &tmp_set, &setWrite, NULL, NULL), "select");
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_set)) {
				if (i == server_fd) {
					newSocket = accept(server_fd, (sockaddr *)address, (socklen_t *) &addrlen);
					check(newSocket, "accept");
					FD_SET(newSocket, &set);
					FD_SET(newSocket, &setWrite);
					if (newSocket > fdmax) { fdmax = newSocket; }
					std::cout << "New connect on fd " << newSocket << std::endl;
				}
				else {
					std::cout << "it is not new connect its a package:" <<  std::endl;
					char buf[1024];
					int rec = recv(i, buf, 1024, 0);
					if (rec == 0) {
						close(i);
						FD_CLR(i, &set);
					} else {
						for (int k = 0; k <= fdmax; k++) {
							if (FD_ISSET(k, &setWrite)) {
								if (k != server_fd) {
									std::string responce ("message from ");
									responce.append(std::to_string(i));
									responce.append(": ");
									responce.append(buf);
									sendBuf(k, responce.c_str(), responce.length());
								}
							}
						}
						std::cout << GREEN << buf << std::endl << RESET;
					}
					bzero(buf, 1024);
				}
			}
		}
	}
}

void sendToAll(int nfds, pollfd fds[], char *buf) {
	int rc = poll(fds, nfds, 0);
	if (rc < 0) {
		perror("poll");
		return ;
	}

	for (int i = 0; i < nfds; i++) {
		if (fds[i].revents == 0) continue;
		std::string responce ("message from ");
		responce.append(std::to_string(i));
		responce.append(": ");
		responce.append(buf);
		responce.append("\n");
		if (fds[i].revents == POLLOUT) {
			send(fds[i].fd, responce.c_str(), responce.length(), 0);
		}
	}
}

void pollLogic(int server_fd, sockaddr_in *address, int addrlen) {
	pollfd fds[200]; // vector??? 
	char buf[1024];
	int timeout = 3 * 60 * 1000; // 3 min
	int nfds = 1, newFd;

	bzero(fds, sizeof(fds));
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;

	while (true) {
		int rc =  poll(fds, nfds, timeout);
		if (rc < 0) {
			perror("poll");
			break ;
		}
		int currentSize = nfds;
		for (int i = 0; i < currentSize; i++) {
			if (fds[i].revents == 0) continue;
			if (fds[i].fd == server_fd) {
				do
				{
					newFd = accept(server_fd, (sockaddr *)address, (socklen_t *) &addrlen);
					fds[nfds].fd = newFd;
					fds[nfds].events = POLLIN;
					nfds++;
					fds[nfds].fd = newFd;
					fds[nfds].events = POLLOUT;
					nfds++;
				} while (newFd != -1);
			}
			else {
				if (fds[i].revents == POLLIN) {
					bzero(buf, 1024);
					int ret = recv(fds[i].fd, buf, 1024, 0);
					if (ret < 0) {
						perror("recv");
						break;
					}
					std::cout << GREEN << buf << RESET << std::endl;
					sendToAll(nfds, fds, buf);
				}
			}
		}
	}


	for (int i = 0; i < nfds; i++) {
		if (fds[i].fd >= 0) {
			close(fds[i].fd);
		}
	}
}

int main(void) {
	sockaddr_in	address;
	int			server_fd;
	int			addrlen		= sizeof(address);

	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	check(server_fd, "socket");
	

	int yes=1;
	check(
		setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)), "setsockopt");
	
	setSockAddr_in(&address);
	check( bind(server_fd, (sockaddr *)&address, sizeof(address)), "bind");
	check( listen(server_fd, 10), "listen");
	
	
	// selectLogic(server_fd, &address, addrlen);
	pollLogic(server_fd, &address, addrlen);
	
	close(server_fd);
	return 0;
}


// listen back_log wtf? 