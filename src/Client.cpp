#include "../include/Client.hpp"
#include <unistd.h>

Client::Client(int fd, Server& server, sockaddr_in& client_addr):
	m_fd(fd),
	m_client_status(0),
	m_server(server),
	m_ip_addr(client_addr.sin_addr.s_addr),
	m_port(client_addr.sin_port),
	m_last_activity(),
	m_request_buffer()
{
	(void)m_fd;
	(void)m_server;
	(void)m_ip_addr;
	(void)m_port;
	std::time(&m_last_activity);
}
Client::~Client()
{}

int Client::get_client_status()
{
	return this->m_client_status;
}

void Client::handle_read()
{
	char buffer[CHUNK_SIZE];
	ssize_t bytes_read = recv(this->m_fd, buffer, CHUNK_SIZE, 0);
	if (bytes_read == 0)
	{
		this->m_client_status = CLIENT_DISCONNECTED;
		return;
	}
	if (bytes_read == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return;
	}
	switch (this->m_read_state)
	{
		case RECV_HEADER:

		case RECV_BODY:
		case RECV_DONE:
	}
	m_request_buffer.push(buffer, bytes_read);
	m_request_buffer.is_header_finished();
}