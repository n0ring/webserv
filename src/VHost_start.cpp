#include "VHost.hpp"

VHost::VHost(void) {
	this->_maxBody = -1;
	this->_port = -1;
	this->_listener = -1;
}

VHost::~VHost(void) {
	close(this->_listener);
}

VHost::VHost(int port, std::string ip, int bl) : _port(port),
		_ip(ip), _backlog(bl) {
}

VHost::VHost(VHost const &other) {
	*this = other;
}

VHost & VHost::operator=(VHost const &other) {
	if (this != &other) {
		this->_port = other._port;
		this->_ip = other._ip;
		this->_serverName = other._serverName;
		this->_backlog = other._backlog;
		this->_maxBody = other._maxBody;
		this->_listener = other._listener;
		this->_address = other._address;
		this->_addrlen = other._addrlen;
		this->locations= other.locations;
	}
	return *this;
}

int VHost::setup(void) {
	this->setupSocket();
	this->setupSockAddr_in();

	int	rec = bind(this->_listener, &_address, this->_addrlen);
	if (rec == -1) {
		perror("Can't bind params: ");
		std::cout << this->_ip << ":" << this->_port << std::endl; 
		return (-1);
	}
	rec = listen(_listener, _backlog);
	if (rec == -1) {
		perror("listen");
		return (-1);
	}
	return this->_listener;
}

void	VHost::setupSocket(void) {
	this->_listener = socket(PF_INET, SOCK_STREAM, 0); // maybe add second socket for udp
	if (this->_listener == -1) {
		perror("socket");
		exit(-1);
	}
	fcntl(_listener, F_SETFL, O_NONBLOCK);
	int yes=1;
	setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
}

void	VHost::setupSockAddr_in(void) {
	sockaddr_in *address;
	
	bzero(&(this->_address), sizeof(this->_address));
	this->_addrlen = (socklen_t) sizeof(this->_address);

	address = reinterpret_cast<sockaddr_in *>(&this->_address);
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = inet_addr(this->_ip.c_str());
	// address->sin_addr.s_addr = htons(INADDR_ANY);
	address->sin_port = htons(this->_port);
}

int		VHost::acceptNewConnection() {
	int newSocket =  accept(this->_listener, &this->_address, &this->_addrlen);
	if (newSocket < 0) {
		return -1;
	}
	std::cout << "accepting connect on fd " << this->_listener << std::endl;
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	return newSocket;
}

void VHost::setNewLocation(std::vector<std::string> params) {
	typedef std::vector<std::string>::size_type size_type;
	location loc;
	for (size_type i = 1; i < params.size() && params[i].compare("{"); i++) {
		loc.names.push_back(params[i]);
	}
	this->locations.push_back(loc);
}

void VHost::setServerParams(std::vector<std::string> params) {
	if (params.size() != 2) {
		std::cout << "incorrect paramr pair" << std::endl;
		return ;
	}
	if (params.front().compare("host") == 0) {
		this->_ip = params.back();
		return ;
	}
	if (params.front().compare("port") == 0) {
		this->_port = std::stoi(params.back());
		return ;
	}
	if (params.front().compare("server_names") == 0) {
		this->_serverName = params.back();
		return ;
	}
	if (params.front().compare("max_client_body_size") == 0) {
		this->_maxBody = std::stoi(params.back());
		return ;
	}
}

void VHost::setLocationParam(std::vector<std::string> inputParams) {
	if (inputParams.size() < 2) return ;

	if (inputParams.front().compare("root") == 0) { // need to del
		this->locations.back().root = inputParams.back();
		truncStr(this->locations.back().root);
		return ;
	}
	if (inputParams.front().compare("index") == 0) {
		this->locations.back().index = inputParams.back();
		return ;
	}
	if (inputParams.front().compare("methods") == 0) {
		for (size_t i = 1; i < inputParams.size(); i++) {
			this->locations.back().methods.push_back(inputParams[i]);
		}
		return ;
	}
	if (inputParams.front().compare("autoindex") == 0) {
		this->locations.back().autoindex = inputParams.back();
		return ;
	}

	if (inputParams.front().compare("cgi") == 0) {
		this->locations.back().cgi = inputParams.back();
		return ;
	}
}

void VHost::validate() {
	// check index in dirs location 
	// check for names > 1 if [0] != *
	// check for file formats != dirs
	// root has to start with /
	// valid methods (only 3)
	// not same ports
	bool isValid = true;
	if (this->_ip.empty()) {
		isValid = false;
		std::cerr << "Ip address not found" << std::endl;		
	}
	if (this->_port == -1) {
		isValid = false;
		std::cerr << "Port not found" << std::endl;		
	}
	if (this->_maxBody == -1) {
		isValid = false;
		std::cerr << "Max body size not found" << std::endl;		
	}
	if (this->locations.size() == 0) {
		isValid = false;
		std::cerr << "Locations not found" << std::endl;		
	}
	if (!isValid) {
		exit(-1);
	}
}

void VHost::toString() {
	std::cout << "<--Server Config-->" << std::endl;
	std::cout << "Port: " << this->_port << std::endl;
	std::cout << "Ip: " << this->_ip << std::endl;
	std::cout << "Max body: " << this->_maxBody << std::endl;
	std::cout << "Backlog: " << this->_backlog << std::endl;
	std::cout << "Listener: " << this->_listener << std::endl;
	for (std::vector<location>::size_type i = 0; i < this->locations.size(); i++) {
		this->locations[i].toString();
	}
}