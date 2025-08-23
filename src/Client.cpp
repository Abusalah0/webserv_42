#include "../include/Client.hpp"
#include <unistd.h>

Client::Client(int fd, Server& server, sockaddr_in& client_addr):
	m_fd(fd),
	m_server(server),
	m_ip_addr(client_addr.sin_addr.s_addr),
	m_port(client_addr.sin_port)
{
	(void)m_fd;
	(void)m_server;
	(void)m_ip_addr;
	(void)m_port;
}

Client::~Client()
{}