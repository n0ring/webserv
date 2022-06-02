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

		void	onClientConnect(ServerConfig &serverConfig, std::vector<pollfd>& fds,
							std::vector<pollfd>::iterator& iter);
		void	onClientDisconnect(std::vector<pollfd>::iterator& iter, std::vector<pollfd> &fds);
		void	onClientDataExchange(std::vector<pollfd>::iterator& iter,
									ServerConfig &vHost);
		void	onClientError(int event);
};