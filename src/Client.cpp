#include "../include/Client.hpp"
#include <unistd.h>

Client::Client(int fd, Server* server, sockaddr_in& client_addr):
	m_fd(fd),
	m_client_status(0),
	m_process_state(PROCESS_HEADER),
	m_base_server(server),
	m_target_server(0),
	m_target_location(0),
	m_ip_addr(client_addr.sin_addr.s_addr),
	m_port(client_addr.sin_port),
	m_request_buffer(),
	m_response_buffer(),
	m_header()
{
	(void)m_fd;
	(void)m_base_server;
	(void)m_target_server;
	(void)m_target_location;
	(void)m_ip_addr;
	(void)m_port;
	std::time(&m_last_activity);
}

Client::Client()
{}

Client::~Client()
{}

int Client::get_client_status()
{
	return this->m_client_status;
}

void Client::handle_read()
{
	if (this->m_client_status != CLIENT_ALIVE)
		return;
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
	this->m_request_buffer.push(buffer, bytes_read);
}

void Client::handle_send()
{
	if (this->m_client_status != CLIENT_ALIVE || !this->m_response_buffer.size())
		return;
	std::string buffer = this->m_response_buffer.pop();
	ssize_t bytes_sent = send(this->m_fd, buffer.c_str(), buffer.size(), 0);
	if (bytes_sent == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return;
	}
}

void Client::generate_error(const WebservExceptions::HTTPException& e)
{

}

void Client::process()
{
	if (this->m_client_status != CLIENT_ALIVE)
		return;
	try
	{
		switch (this->m_process_state)
		{	
			case PROCESS_HEADER:
				if (this->m_request_buffer.is_header_finished())
				{
					this->m_header.clear();
					this->m_request_buffer.isolate_header();
					std::string line = this->m_request_buffer.pop();
					this->m_header.parse(line);
					if (this->m_header.get_content_length() || this->m_header.is_chunked())
						this->m_process_state = PROCESS_BODY;
					else
						this->m_process_state = PROCESS_REQUEST;
				}
				if (this->m_request_buffer.size() > 1)
				{
					throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
				}
			case PROCESS_BODY:
				
		}
	}
	catch (const WebservExceptions::HTTPException& e)
	{
		generate_error(e);
		this->m_client_status = CLIENT_DONE;
	}
}