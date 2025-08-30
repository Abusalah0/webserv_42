#include "../include/Client.hpp"
#include <unistd.h>
#include <iostream>

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
	m_header(),
	m_body()
{
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
	std::string buffer = this->m_response_buffer.pull(CHUNK_SIZE);
	ssize_t bytes_sent = send(this->m_fd, buffer.c_str(), buffer.size(), 0);
	if (bytes_sent == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return;
	}
}

void Client::generate_error(const WebservExceptions::HTTPException& e)
{
	(void)e;
}

void Client::process_header()
{
	if (this->m_request_buffer.is_header_finished())
	{
		this->m_header.clear();
		std::string input = this->m_request_buffer.pull_header();
		this->m_header.parse(input);
		if (this->m_header.get_content_length())
			this->m_process_state = PROCESS_BODY;
		else if (this->m_header.is_chunked())
			this->m_process_state = PROCESS_BODY_CHUNKED_SIZE;
		else
			this->m_process_state = PROCESS_REQUEST;
	}
	if (this->m_request_buffer.size() > CHUNK_SIZE)
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}
}

void Client::process_body()
{
	if (this->m_request_buffer.size() >= this->m_header.get_content_length())
	{
		this->m_body = this->m_request_buffer.pull(this->m_header.get_content_length());
		this->m_process_state = PROCESS_REQUEST;
	}
}

void Client::process_body_chunked_size()
{
	if (this->m_request_buffer.is_clrf_found())
	{
		std::string chunk_size_str = this->m_request_buffer.pull_encoded();
		this->m_request_buffer.erase(2);
		this->m_chunk_size = parse_chunk_size(chunk_size_str);
		if (this->m_chunk_size)
			this->m_process_state = PROCESS_BODY_CHUNKED_DATA;
		else
			this->m_process_state = PROCESS_BODY_CHUNKED_END;
	}
}

void Client::process_body_chunked_data()
{
	if (this->m_request_buffer.size() >= this->m_chunk_size + 2)
	{
		std::string chunk_data = this->m_request_buffer.pull(this->m_chunk_size + 2);
		if (chunk_data.compare(this->m_chunk_size, 2, "\r\n"))
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		chunk_data.erase(this->m_chunk_size);
		this->m_body.append(chunk_data);
		this->m_process_state = PROCESS_BODY_CHUNKED_SIZE;
	}
}

void Client::process_body_chunked_end()
{
	if (this->m_request_buffer.size() >= 2)
	{
		std::string clrf = this->m_request_buffer.pull(2);
		if (clrf.compare("\r\n"))
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		this->m_process_state = PROCESS_REQUEST;
	}
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
				process_header();
				break;
			case PROCESS_BODY:
				process_body();
				break;
			case PROCESS_BODY_CHUNKED_SIZE:
				process_body_chunked_size();
				break;
			case PROCESS_BODY_CHUNKED_DATA:
				process_body_chunked_data();
				break;
			case PROCESS_BODY_CHUNKED_END:
				process_body_chunked_end();
				break;
			case PROCESS_REQUEST:
				this->m_header.debug();
				std::cout << "---------------BODY---------------" << std::endl;
				std::cout << this->m_body << std::endl;
				this->m_process_state = PROCESS_HEADER;
				break;
		}
	}
	catch (const WebservExceptions::HTTPException& e)
	{
		if (e.get_error_code() == HTTP_BAD_REQUEST)
			this->m_client_status = CLIENT_DONE;
		generate_error(e);
		std::cerr << e.what() << std::endl;
	}
}