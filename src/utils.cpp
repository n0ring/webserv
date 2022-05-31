#include "utils.hpp"

pollfd make_fd(int fd, int event) {
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = event;
	newfd.revents = 0;
	return newfd;
}