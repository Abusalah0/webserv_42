#include "../include/Client.hpp"
#include <ServerContainer.hpp>
#include <unistd.h>
#include <iostream>

Client::Client(int fd,
	ServerContainer* server_container,
	Server* server,
	const std::pair<std::string, std::string>* listen_entry):
	m_listen_fd(fd),
	m_client_status(CLIENT_ALIVE),
	m_process_state(PROCESS_HEADER),
	m_current_scope(SCOPE_BASE_SERVER),
	m_base_server(server),
	m_target_server(0),
	m_target_location(0),
	m_server_container(server_container),
	m_request_buffer(),
	m_response_buffer(),
	m_header(),
	m_body(),
	m_listen_entry(listen_entry)
{
	(void)m_target_location;
	(void)m_base_server;
	(void)m_target_server;
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
	ssize_t bytes_read = recv(this->m_listen_fd, buffer, CHUNK_SIZE, 0);
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
	if (this->m_client_status > CLIENT_DONE || !this->m_response_buffer.size())
		return;
	std::string buffer = this->m_response_buffer.pull(CHUNK_SIZE);
	ssize_t bytes_sent = send(this->m_listen_fd, buffer.c_str(), buffer.size(), 0);
	if (bytes_sent == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return;
	}
}

void Client::process_header()
{
	if (this->m_request_buffer.is_header_finished())
	{
		std::string input = this->m_request_buffer.pull_header();
		this->m_header.parse_request(input);
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
		this->m_header.set_content_length(this->m_body.size());
		this->m_process_state = PROCESS_REQUEST;
	}
}

std::string generate_fallback_body(const std::string& msg)
{
	std::string body =
		"<html>\n"
		"<head><title>{template}</title></head>\n"
		"<body>\n"
		"<center><h1>{template}</h1></center>\n"
		"<hr><center>webserv/1.0</center>\n"
		"</body>\n"
		"</html>";
	std::string str_template = "{template}";
	size_t pos = body.find(str_template);
	body.erase(pos, str_template.size());
	body.insert(pos, msg);
	pos = body.find(str_template, pos + msg.size());
	body.erase(pos, str_template.size());
	body.insert(pos, msg);
	return body;
}

void Client::fallback_generate_error(const WebservExceptions::HTTPException& e)
{
	std::string body = generate_fallback_body(e.what());
	this->m_header.clear();
	this->m_header.set_content_length(body.size());
	this->m_header.generate_response_fields(this->m_client_status, e.what(), false);
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
}

void Client::generate_error(const WebservExceptions::HTTPException& e)
{
	std::string error_page;

	try
	{
		if (this->m_current_scope == SCOPE_BASE_SERVER)
			error_page = this->m_base_server->get_error_page(e.get_error_code());
		else if (this->m_current_scope == SCOPE_TARGET_SERVER)
			error_page = this->m_target_server->get_error_page(e.get_error_code());
		else
			error_page = this->m_target_location->get_error_page(e.get_error_code());
	}
	catch(const WebservExceptions::HTTPException& inner_e)
	{
		fallback_generate_error(inner_e);
	}
	catch(const WebservExceptions::NoAvailablePage& inner_e)
	{
		fallback_generate_error(e);
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
				std::cout << this->m_listen_entry->first << ':' << this->m_listen_entry->second << std::endl;
				std::cout << this->m_header.get_virtual_host() << std::endl;
				const Server& target_server = this->m_server_container->get_best_server(
					this->m_listen_entry->first, this->m_listen_entry->second, this->m_header.get_virtual_host()
				);
				(void)target_server;
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
		reset_client_state();
	}
}

void Client::reset_client_state()
{
	m_process_state = PROCESS_HEADER;
	m_current_scope = SCOPE_BASE_SERVER;
	m_header.clear();
	m_body.clear();
}