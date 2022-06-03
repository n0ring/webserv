#pragma once

#include <map>
#include <vector>
#include <poll.h>
#include "VHost.hpp"
#include "Connection.hpp"
#include "utils.hpp"

class ConnectionPool {
	private:
		std::map<int, Connection>	_pool;

	public:
		ConnectionPool();
		~ConnectionPool(void);

		void	onClientConnect(VHost &serverConfig, std::vector<pollfd>& fds,
							std::vector<pollfd>::iterator& iter);
		void	onClientDisconnect(std::vector<pollfd>::iterator& iter, std::vector<pollfd> &fds);
		void	onClientDataExchange(std::vector<pollfd>::iterator& iter,
									VHost &vHost);
		void	onClientError(int event);

		int		getConnectionListener(int fd);
};