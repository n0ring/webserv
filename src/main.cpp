#include "Server.hpp"

int main(int argc, char **argv) {
	Server server;

	if (argc != 2) {
		server.start("config.conf");
	}
	else {
		server.start(argv[1]);
	}
	return 0;
}

