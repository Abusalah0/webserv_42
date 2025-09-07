#include "../include/Client.hpp"
#include "../include/ServerContainer.hpp"
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include "../include/CommonUtils.hpp"

Client::Client(int fd,
	ServerContainer* server_container,
	Server* server,
	const std::pair<std::string, std::string>* server_addr,
	const std::pair<std::string, std::string>& client_addr):
	m_listen_fd(fd),
	m_file_fd(-1),
	m_body_size(0),
	m_client_status(CLIENT_ALIVE),
	m_connection_type(CONNECTION_KEEP_ALIVE),
	m_process_state(PROCESS_HEADER),
	m_base_server(server),
	m_target_block(server),
	m_server_container(server_container),
	m_request_buffer(),
	m_response_buffer(),
	m_cgi_buffer(),
	m_header(),
	m_cgi_header(),
	m_body(),
	m_script_name(),
	m_server_addr(server_addr),
	m_client_addr(client_addr),
	m_cgi_handler(server_container, this),
	m_cgi_header_finished(false)
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
	this->m_request_buffer.header_lf_to_clrf();
	if (this->m_request_buffer.is_header_finished())
	{
		std::string input = this->m_request_buffer.pull_header();
		this->m_header.parse_request(input);
		this->m_connection_type = this->m_header.get_connection_type();
		this->m_process_state = PROCESS_SELECT_TARGET;
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
	this->m_header.generate_response_fields(this->m_connection_type, HTTP_OK_MSG, false, "text/html");
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::handle_index()
{
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
		prep_process_file_body(path);
	else if (is_http_target_dir(path))
	{
		const std::string& root = location_target->get_root();
		std::string location = path.substr(root.size());
		generate_error(HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG, location);
	}
	else
	{
		throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
	}
}

void validate_cgi_files_permissions(const std::string& full_path, const std::string& cgi_pass)
{
	struct stat statbuf;
	if (stat(full_path.c_str(), &statbuf))
	{
		if (errno == ENOENT)
			throw WebservExceptions::CGINotFound();
		handle_http_file_errno();
	}
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	if (access(full_path.c_str(), R_OK))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	if (stat(cgi_pass.c_str(), &statbuf))
	{
		if (errno == ENOENT)
			throw WebservExceptions::CGINotFound();
		handle_http_file_errno();
	}
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	if (access(cgi_pass.c_str(), X_OK))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
}

void Client::handle_cgi()
{
	const Location* location_target;
	std::string& target = this->m_header.get_target();
	location_target = dynamic_cast<const Location*>(this->m_target_block);
	if (!location_target || !location_target->is_cgi_requirments(target))
		throw WebservExceptions::CGINotFound();
	std::string full_path = concat_path(location_target->get_root(), target);
	const std::string& cgi_pass = location_target->get_cgi_pass();
	validate_cgi_files_permissions(full_path, cgi_pass);
	this->m_script_name = full_path.substr(location_target->get_root().size());
	this->m_cgi_handler.init_cgi(cgi_pass, full_path);
	this->m_process_state = PROCESS_CGI_BEGINNING;
}

void Client::handle_file_upload()
{
	const Location* location = dynamic_cast<const Location*>(this->m_target_block);
	if (!location)
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	std::string& target = this->m_header.get_target();
	std::string path = concat_path(location->get_root(), target);
	size_t pos = path.rfind('/');
	std::string dir = path.substr(0, pos);

	if (!access(path.c_str(), F_OK))
	{
		struct stat statbuf;
		if (stat(path.c_str(), &statbuf))
			handle_http_file_errno();
		if (!S_ISREG(statbuf.st_mode))
			throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
		if (access(dir.c_str(), W_OK))
			throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
	
	this->m_file_fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (this->m_file_fd == -1)
		handle_http_file_errno();
	this->m_server_container->add_to_poll(this->m_file_fd);
	this->m_process_state = PROCESS_FILE_UPLOAD;
}

void Client::handle_file_delete()
{
	std::string& target = this->m_header.get_target();
	std::string path = concat_path(this->m_target_block->get_root(), target);

	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
		handle_http_file_errno();
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);

	if (std::remove(path.c_str()))
		handle_http_file_errno();

	this->m_header.generate_response_fields(HTTP_NO_CONTENT, HTTP_NO_CONTENT_MSG, false);
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::process_request_get()
{
	std::string& target = this->m_header.get_target();
	std::string path = concat_path(this->m_target_block->get_root(), target);
	
	if (str_back(target) != '/')
	{
		try
		{
			handle_cgi();
		}
		catch (const WebservExceptions::CGINotFound& e)
		{
			direct_serve(this->m_target_block);
		}
	}
	else
	{		
		if (is_http_target_dir(path))
		{
			if (this->m_target_block->get_auto_index())
			{
				std::deque<AutoIndexEntry> entries = generate_autoindex_entries(
					this->m_target_block->get_root(), target
				);
				serve_autoindex(entries);
			}
			else
			{
				try
				{
					handle_index();
				}
				catch (const WebservExceptions::NoAvailablePage& e)
				{
					throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
				}
			}
		}
		else
			throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
}

void Client::process_request_post()
{
	std::string& target = this->m_header.get_target();
	
	if (str_back(target) == '/')
		throw WebservExceptions::HTTPException(HTTP_NOT_IMPLEMENTED);
	try
	{
		handle_cgi();
	}
	catch (const WebservExceptions::CGINotFound& e)
	{
		handle_file_upload();
	}
}

void Client::process_request_delete()
{
	std::string& target = this->m_header.get_target();
	
	if (str_back(target) == '/')
	{
		throw WebservExceptions::HTTPException(HTTP_NOT_IMPLEMENTED);
	}
	try
	{
		handle_cgi();
	}
	catch (const WebservExceptions::CGINotFound& e)
	{
		handle_file_delete();
	}
}

void Client::process_request_any()
{
	try
	{
		handle_cgi();
	}
	catch (const WebservExceptions::CGINotFound& e)
	{
		throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
	}
}

void Client::process_request()
{
	if (this->m_connection_type == CONNECTION_CLOSE)
	{
		this->m_request_buffer.erase(this->m_request_buffer.size());
		this->m_client_status = CLIENT_DONE;
	}
	std::string& request_method = this->m_header.get_request_method();
	if (request_method == "GET")
		process_request_get();
	else if (request_method == "POST")
		process_request_post();
	else if (request_method == "DELETE")
		process_request_delete();
	else
		process_request_any();
}

void Client::select_target()
{
	const Server* server = &this->m_server_container->get_best_server(
		this->m_server_addr->first, this->m_server_addr->second, this->m_header.get_virtual_host()
	);
	if (!server->get_server_names().size())
		this->m_server_name = this->m_server_addr->first;
	else
		this->m_server_name = this->m_header.get_virtual_host();
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
		"<hr><center>";
	body.append(SERVER_SOFTWARE);
	body.append(
		"</center>\n"
		"</body>\n"
		"</html>\n"
	);
	replace_template_str(body, str_template, msg);
	return body;
}

void Client::fallback_generate_error(const std::string& msg, const std::string& location)
{
	std::string body = generate_fallback_body(msg);
	this->m_header.clear();
	this->m_header.set_content_length(body.size());
	this->m_header.generate_response_fields(this->m_connection_type, msg, false, "text/html");
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
		if (!error_page.empty() && error_page[0] == '/')
			prep_process_file_body(error_page, msg);
		else
			fallback_generate_error(HTTP_FOUND_MSG, error_page);
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
	this->m_header.generate_response_fields(this->m_connection_type, msg, false, media_type);
	std::string response_header = this->m_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_process_state = PROCESS_FILE_BODY;
}

void Client::process_cgi_beginning()
{
	if (this->m_cgi_handler.is_write_ready())
	{
		if (!this->m_body.empty())
		{
			std::string chunk = this->m_body.substr(0, CHUNK_SIZE);
			this->m_body.erase(0, CHUNK_SIZE);
			this->m_cgi_handler.write_cgi(this->m_body);
			if (this->m_body.empty())
				this->m_cgi_handler.close_write();
		}
		else
			this->m_cgi_handler.close_write();
	}
	if (this->m_cgi_handler.is_read_ready())
	{
		if (!this->m_cgi_header_finished)
		{
			std::string data = this->m_cgi_handler.read_cgi();
			this->m_cgi_buffer.push(data.c_str(), data.size());
			this->m_cgi_buffer.header_lf_to_clrf();
			if (this->m_cgi_buffer.is_header_finished())
			{
				std::string header_str = this->m_cgi_buffer.pull_header();
				this->m_cgi_header.set_chunked();
				this->m_cgi_header.parse_response(header_str);
				this->m_header.clear();
				this->m_header.ignore_content_len_field();
				this->m_header.generate_response_fields(
					this->m_connection_type, HTTP_OK_MSG, this->m_cgi_header.is_chunked(), "text/html"
				);
				this->m_header.parse_response(header_str);
				header_str = this->m_header.generate_response_header();
				this->m_response_buffer.push(header_str.c_str(), header_str.size());
				this->m_cgi_header_finished = true;
			}
			else if (this->m_cgi_buffer.size() > CHUNK_SIZE)
				throw WebservExceptions::HTTPException(HTTP_CONTENT_TOO_LARGE);
		}
	}
	if (!this->m_cgi_handler.is_read_open() && !this->m_cgi_header_finished)
	{
		this->m_cgi_handler.clean_handler();
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
	}
	if (this->m_cgi_header_finished && this->m_body.empty())
	{
		this->m_body_size = 0;
		this->m_process_state = PROCESS_CGI_READ;
	}
	if (this->m_cgi_handler.is_timeout())
		throw WebservExceptions::HTTPException(HTTP_GATEWAY_TIMEOUT);
}

void Client::handle_cgi_read_chunked(std::string& data)
{
	std::string str_size = ul_to_hex(data.size());
	this->m_response_buffer.push(str_size.c_str(), str_size.size());
	this->m_response_buffer.push("\r\n", 2);
	this->m_response_buffer.push(data.c_str(), data.size());
	this->m_response_buffer.push("\r\n", 2);
}

void Client::handle_cgi_read(std::string& data)
{
	this->m_body_size += data.size();
	if (this->m_body_size > this->m_cgi_header.get_content_length())
		data = data.substr(0, this->m_body_size - this->m_cgi_header.get_content_length());
	this->m_response_buffer.push(data.c_str(), data.size());
	if (this->m_body_size >= this->m_cgi_header.get_content_length())
		reset_client_state();
}

void Client::process_cgi_read()
{
	if (this->m_cgi_buffer.size())
	{
		std::string data = this->m_cgi_buffer.pull(this->m_cgi_buffer.size());
		if (this->m_cgi_header.is_chunked())
			handle_cgi_read_chunked(data);
		else
			handle_cgi_read(data);
	}
	if (!this->m_cgi_handler.is_read_open())
		reset_client_state();
	else
	{
		if (this->m_cgi_handler.is_read_ready())
		{
			std::string data = this->m_cgi_handler.read_cgi();
			if (this->m_cgi_header.is_chunked())
				handle_cgi_read_chunked(data);
			else
				handle_cgi_read(data);
		}
		if (this->m_cgi_handler.is_timeout())
			throw WebservExceptions::HTTPException(HTTP_GATEWAY_TIMEOUT);
	}
	if (this->m_cgi_handler.is_dead() && !this->m_cgi_header.is_chunked())
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
}

void Client::process_file_upload()
{
	size_t bytes_to_write = this->m_body.size();
	if (bytes_to_write > CHUNK_SIZE)
		bytes_to_write = CHUNK_SIZE;
	ssize_t res = write(this->m_file_fd, this->m_body.c_str(), bytes_to_write);
	if (res == -1)
		WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	this->m_body.erase(0, bytes_to_write);
	if (this->m_body.empty())
	{
		std::string& target = this->m_header.get_target();
		std::string path = concat_path(this->m_target_block->get_root(), target);
		std::string body = "File uploaded successfully to: " + path + "\n";
		this->m_header.set_content_length(body.size());
		this->m_header.generate_response_fields(HTTP_CREATED, HTTP_CREATED_MSG, false, "text/plain");
		std::string response_header = this->m_header.generate_response_header();
		this->m_response_buffer.push(response_header.c_str(), response_header.size());
		this->m_response_buffer.push(body.c_str(), body.size());
		this->m_response_buffer.create_barrier();
		reset_client_state();
	}
}

void Client::process()
{
	if (this->m_client_status > CLIENT_DONE)
		return;
	try
	{
		switch (this->m_process_state)
		{	
			case PROCESS_HEADER:
				process_header();
				break;
			case PROCESS_SELECT_TARGET:
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
			case PROCESS_CGI_BEGINNING:
				process_cgi_beginning();
				break;
			case PROCESS_CGI_READ:
				process_cgi_read();
				break;
			case PROCESS_FILE_UPLOAD:
				process_file_upload();
				break;
		}
	}
	catch (const WebservExceptions::HTTPException& e)
	{
		if (this->m_process_state == PROCESS_CGI_READ)
		{
			this->m_client_status = CLIENT_DISCONNECTED;
			return;
		}
		if (e.get_error_code() == HTTP_BAD_REQUEST)
		{
			this->m_request_buffer.erase(this->m_request_buffer.size());
			this->m_client_status = CLIENT_DONE;
			this->m_connection_type = CONNECTION_CLOSE;
		}
		generate_error(e.get_error_code(), e.what(), "");
	}
}

void Client::reset_client_state()
{
	close_file();
	this->m_process_state = PROCESS_HEADER;
	this->m_target_block = this->m_base_server;
	this->m_body_size = 0;
	this->m_cgi_header_finished = false;
	this->m_header.clear();
	this->m_cgi_header.clear();
	this->m_body.clear();
	this->m_cgi_buffer.erase(this->m_cgi_buffer.size());
	this->m_cgi_handler.clean_handler();
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

time_t Client::get_last_activity() const
{
	return this->m_last_activity;
}

HTTPHeader& Client::get_header()
{
	return this->m_header;
}

const std::pair<std::string, std::string>& Client::get_client_addr() const
{
	return this->m_client_addr;
}

const std::pair<std::string, std::string>& Client::get_server_addr() const
{
	return *this->m_server_addr;
}

const std::string& Client::get_script_name()
{
	return this->m_script_name;
}

const std::string& Client::get_server_name()
{
	return this->m_server_name;
}