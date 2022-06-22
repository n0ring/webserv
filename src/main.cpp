#include "Server.hpp"

int main(int argc, char **argv) {
	Server server;

	if (argc != 2) {
		server.start("/Users/namina/21/webServer/config.conf");
	}
	else {
		server.start(argv[1]);
	}
	return 0;
}

