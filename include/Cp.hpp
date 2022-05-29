#pragma once

#include <map>
#include <vector>
#include <poll.h>
#include "ServerConfig.hpp"
#include "Connection.hpp"
#include "utils.hpp"


class Cp {
	private:
		std::map<int, Connection>	_pool;

	public:
		Cp();
		~Cp(void);

		void onClientConnect(ServerConfig &serverConfig, std::vector<pollfd>& fds);
		void onClientDisconnect(pollfd& pfd);


};