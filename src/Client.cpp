#include "../include/Client.hpp"
#include "../include/ServerContainer.hpp"
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include "../include/CommonUtils.hpp"

Client::Client(int fd,
	ServerContainer* server_container,
	Server* server,
	const std::pair<std::string, std::string>* listen_entry):
	m_listen_fd(fd),
	m_file_fd(-1),
	m_body_size(0),
	m_client_status(CLIENT_ALIVE),
	m_process_state(PROCESS_HEADER),
	m_base_server(server),
	m_target_block(0),
	m_server_container(server_container),
	m_request_buffer(),
	m_response_buffer(),
	m_header(),
	m_body(),
	m_listen_entry(listen_entry)
{
	this->m_last_activity = std::time(0);
}

Client::Client()
{}

Client::~Client()
{
	close_file();
}

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
	this->m_last_activity = std::time(0);
	
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
	if (this->m_client_status == CLIENT_DONE && !this->m_request_buffer.size())
	{
		this->m_client_status = CLIENT_DISCONNECTED;
		return;
	}
	this->m_last_activity = std::time(0);
}

void Client::process_header()
{
	if (this->m_request_buffer.is_header_finished())
	{
		std::string input = this->m_request_buffer.pull_header();
		this->m_header.parse_request(input);
		this->m_process_state = SELECT_TARGET;
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
		{
			this->m_body_size += this->m_chunk_size;
			if (this->m_body_size > this->m_target_block->get_client_max_body_size())
				throw WebservExceptions::HTTPException(HTTP_CONTENT_TOO_LARGE);
			this->m_process_state = PROCESS_BODY_CHUNKED_DATA;
		}
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

void Client::serve_autoindex(const std::deque<AutoIndexEntry>& entries)
{
	std::string body = "<html>\n"
		"<head><title>Index of {template}</title></head>\n"
		"<body>\n"
		"<h1>Index of {template}</h1><hr><pre>\n";
	replace_template_str(body, str_template, this->m_header.get_target());
	for (size_t i = 0; i < entries.size(); i++)
	{
		const AutoIndexEntry& entry = entries[i];
		std::string entry_html = "<a href=\"{template}\">{template}</a>";
		replace_template_str(entry_html, str_template, entry.ent_name);
		if (!S_ISDIR(entry.statbuf.st_mode))
		{
			std::string date = generate_autoindex_date();
			entry_html.push_back(' ');
			entry_html.append(date);
			entry_html.push_back(' ');
			entry_html.append(ul_to_str(entry.statbuf.st_size));
		}
		entry_html.push_back('\n');
		body.append(entry_html);
	}
	body.append(
		"</pre><hr></body>\n"
		"</html>\n"
	);
	this->m_header.set_content_length(body.size());
	this->m_header.generate_response_fields(this->m_client_status, HTTP_OK_MSG, false, "text/html");
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::handle_index()
{
	std::string& request_method = this->m_header.get_request_method();
	if (request_method != "GET")
		throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
	IndexEntry index_entry = this->m_target_block->get_index_page(this->m_header.get_target());
	if (index_entry.is_dir)
	{
		const std::string& root = this->m_target_block->get_root();
		std::string location = index_entry.path.substr(root.size());
		generate_error(HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG, location);
	}
	else
		prep_process_file_body(index_entry.path);
}

void Client::direct_serve(const BaseBlock* location_target)
{
	std::string& target = this->m_header.get_target();
	std::string path = concat_path(location_target->get_root(), target);
	if (is_http_target_file(path))
	{
		prep_process_file_body(path);
		return;
	}
	else if (is_http_target_dir(path))
	{
		const std::string& root = location_target->get_root();
		std::string location = path.substr(root.size());
		generate_error(HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG, location);
	}
	else
		throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
}

void Client::process_request()
{
	std::string& target = this->m_header.get_target();
	std::string path = concat_path(this->m_target_block->get_root(), target);
	if (str_back(this->m_header.get_target()) != '/')
		direct_serve(this->m_target_block);
	else
	{
		if (is_http_target_dir(path))
		{
			if (this->m_target_block->get_auto_index())
			{
				std::deque<AutoIndexEntry> entries = generate_autoindex_entries(
					this->m_target_block->get_root(), this->m_header.get_target()
				);
				serve_autoindex(entries);
			}
			else
				throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
		}
		else
			handle_index();
	}
}

void Client::select_target()
{
	const Server* server = &this->m_server_container->get_best_server(
		this->m_listen_entry->first, this->m_listen_entry->second, this->m_header.get_virtual_host()
	);
	this->m_target_block = server;
	try
	{
		const Location* location = &server->match_location(this->m_header.get_target());
		this->m_target_block = location;
		if (!location->is_method_allowed(this->m_header.get_request_method()))
			throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
	}
	catch (const WebservExceptions::LocationNotFound& e)
	{
	}
	if (this->m_header.get_content_length())
	{
		if (this->m_header.get_content_length() > this->m_target_block->get_client_max_body_size())
			throw WebservExceptions::HTTPException(HTTP_CONTENT_TOO_LARGE);
		this->m_process_state = PROCESS_BODY;
	}
	else if (this->m_header.is_chunked())
		this->m_process_state = PROCESS_BODY_CHUNKED_SIZE;
	else
		this->m_process_state = PROCESS_REQUEST;
}

void Client::process_file_body()
{
	char buffer[CHUNK_SIZE];
	ssize_t bytes_read = read(this->m_file_fd, buffer, CHUNK_SIZE);
	if (bytes_read == 0)
	{
		this->m_response_buffer.create_barrier();
		reset_client_state();
		return;
	}
	if (bytes_read == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		close_file();
		return;
	}
	this->m_response_buffer.push(buffer, bytes_read);
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
		"</html>\n";
	replace_template_str(body, str_template, msg);
	return body;
}

void Client::fallback_generate_error(const std::string& msg, const std::string& location)
{
	std::string body = generate_fallback_body(msg);
	this->m_header.clear();
	this->m_header.set_content_length(body.size());
	this->m_header.generate_response_fields(this->m_client_status, msg, false, "text/html");
	if (!location.empty())
	{
		HTTPHeaderField field;
		field.name = "Location";
		field.value = location;
		this->m_header.add_field(field);
	}
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::generate_error(ushort code, const std::string& msg, const std::string& location)
{
	std::string error_page;

	try
	{
		error_page = this->m_target_block->get_error_page(code);
		if (error_page.size() && error_page[0] != '/')
			fallback_generate_error(HTTP_FOUND_MSG, error_page);
		else
			prep_process_file_body(error_page, msg);
	}
	catch(const WebservExceptions::HTTPException& e)
	{
		fallback_generate_error(e.what(), location);
	}
	catch(const WebservExceptions::NoAvailablePage& e)
	{
		fallback_generate_error(msg, location);
	}
}

void Client::prep_process_file_body(const std::string& file_path, const std::string& msg)
{
	struct stat statbuf;
	if (stat(file_path.c_str(), &statbuf))
		handle_http_file_errno();
	this->m_header.set_content_length(statbuf.st_size);
	this->m_file_fd = open(file_path.c_str(), O_RDONLY);
	if (this->m_file_fd == -1)
		handle_http_file_errno();
	this->m_server_container->add_to_poll(this->m_file_fd);
	const char* media_type = get_media_type(file_path);
	this->m_header.generate_response_fields(this->m_client_status, msg, false, media_type);
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_process_state = PROCESS_FILE_BODY;
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
			case SELECT_TARGET:
				select_target();
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
				process_request();
				break;
			case PROCESS_FILE_BODY:
				process_file_body();
				break;
		}
	}
	catch (const WebservExceptions::HTTPException& e)
	{
		if (e.get_error_code() == HTTP_BAD_REQUEST)
			this->m_client_status = CLIENT_DONE;
		generate_error(e.get_error_code(), e.what(), "");
	}
}

void Client::reset_client_state()
{
	close_file();
	this->m_process_state = PROCESS_HEADER;
	this->m_body_size = 0;
	m_header.clear();
	m_body.clear();
}

void Client::close_file()
{
	if (this->m_file_fd != -1)
	{
		close(this->m_file_fd);
		this->m_server_container->remove_from_poll(this->m_file_fd);
		this->m_file_fd = -1;
	}
}

time_t Client::get_last_activity()
{
	return this->m_last_activity;
}