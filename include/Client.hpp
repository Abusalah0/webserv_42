#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Server.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

class Client
{
	private:
		int m_fd;
		Server& m_server;
		in_addr_t m_ip_addr;
		in_port_t m_port;
	public:
		Client(int fd, Server& server, sockaddr_in& client_addr);
		~Client();
};

#endif