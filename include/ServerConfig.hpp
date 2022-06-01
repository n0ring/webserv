#pragma once
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <vector>
 #include <arpa/inet.h>  // inet_addr



struct location {
	bool						isFormats;
	std::vector<std::string>	names;
	std::string					root;
	std::vector<std::string>	methods; // ints
	std::string					autoindex;
};

class ServerConfig {
	private:
		int			_port; // form cfg
		std::string	_ip; // from cfg
		int 		_backlog; // from cfg
		int			_listener;
		sockaddr	_address; 
		socklen_t	_addrlen;
		

		
		void	setupSocket(void);
		void	setupSockAddr_in(void);


	public:
		ServerConfig(void);
		ServerConfig(int port, std::string ip, int bl);
		ServerConfig(ServerConfig const &other);
		ServerConfig &operator=(ServerConfig const &other);
		~ServerConfig(void);

		int	setup(void);
		int	getListener(void) const;
		int	acceptNewConnection();

};