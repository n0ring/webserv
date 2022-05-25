#include "Select.hpp"

Select::Select(Server &server) : _server(server) {
	FD_ZERO(&this->masterFd);
	FD_ZERO(&this->readFd);
	FD_ZERO(&this->writeFd);
	FD_SET(this->_server.getListener(), &masterFd);
}

Select::~Select(void) {}

void Select::launch(void) {
	fd_set	tmpMaster, tmpRead;
	int		fdMax = this->_server.getListener();

	while (true) {
		tmpMaster = this->masterFd;
		tmpRead = this->readFd;
		select(fdMax + 1, &tmpMaster, &tmpRead, NULL, NULL);
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &tmpMaster)) {
				this->setNewConnection(fdMax);
			}
		}
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &tmpRead)) {
				char buf[1024];
				bzero(buf, 1024);
				recv(i, buf, 1024, 0);
				std::cout << "recieve from " << i << " : " << buf << std::endl;
			}
		}
		sleep(1);
	}
	FD_ZERO(&tmpMaster);
}


void	Select::setNewConnection(int &fdmax) {
	int		newSocket;

	newSocket = this->_server.acceptNewConnection();

	if (newSocket == -1) {
		perror("accept");
		return ;
	}
	FD_SET(newSocket, &this->readFd);
	if (newSocket > fdmax) { fdmax = newSocket; }
	std::cout << "New connect on fd " << newSocket << std::endl;
}