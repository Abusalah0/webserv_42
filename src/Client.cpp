/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:38:11 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/21 18:00:17 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

/**
 * @file Client.cpp
 * @brief Implementation of the Client class for HTTP request/response handling.
 * 
 * This file contains the core HTTP processing logic for the webserv project.
 * The Client class implements a state machine that handles the complete HTTP
 * request/response cycle, including:
 * 
 * - HTTP header parsing and validation
 * - Request body processing (standard and chunked transfer encoding)
 * - HTTP method handling (GET, POST, DELETE)
 * - Static file serving with MIME type detection
 * - Directory listing (autoindex) generation
 * - CGI script execution and output processing
 * - File upload handling (multipart/form-data)
 * - Error response generation with custom error pages
 * - Connection management (keep-alive vs close)
 * - Non-blocking I/O with poll-based event handling
 * 
 * The implementation follows HTTP/1.1 standards and provides robust error
 * handling, security validation, and resource management.
 */

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
	m_req_header(),
	m_resp_header(),
	m_body(),
	m_script_name(),
	m_document_root(),
	m_path_translated(),
	m_cgi_resp_header(),
	m_server_addr(server_addr),
	m_client_addr(client_addr),
	m_cgi_handler(server_container, this),
	m_cgi_header_finished(false),
	m_allowed_methods(0)
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
	return (this->m_client_status);
}

void Client::handle_read()
{
	// Only process reads if client is still alive and active
	if (this->m_client_status != CLIENT_ALIVE)
		return ;
	
	char buffer[CHUNK_SIZE];
	// Attempt to read data from the client socket (non-blocking)
	ssize_t bytes_read = recv(this->m_listen_fd, buffer, CHUNK_SIZE, 0);
	// Connection closed gracefully by client
	if (bytes_read == 0)
	{
		this->m_client_status = CLIENT_DISCONNECTED;
		return ;
	}
	
	// Error occurred during socket read operation
	if (bytes_read == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return ;
	}
	
	// Successfully read data, append to request buffer for processing
	this->m_request_buffer.push(buffer, bytes_read);
	
	// Update activity timestamp for connection timeout tracking
	this->m_last_activity = std::time(0);
}

void Client::handle_send()
{
	// Only send if client is alive/done and there's data to send
	if (this->m_client_status > CLIENT_DONE || !this->m_response_buffer.size())
		return ;
		
	// Pull up to CHUNK_SIZE bytes from response buffer for sending
	std::string buffer = this->m_response_buffer.pull(CHUNK_SIZE);
	
	// Send data to client socket (non-blocking)
	ssize_t bytes_sent = send(this->m_listen_fd, buffer.c_str(), buffer.size(), 0);
	
	// Handle send error (socket closed, network error, etc.)
	if (bytes_sent == -1)
	{
		this->m_client_status = CLIENT_ERROR;
		return ;
	}
	
	// Update activity timestamp for connection timeout tracking
	this->m_last_activity = std::time(0);
}

void Client::process_header()
{
	// Normalize line endings: convert lone LF to CRLF for proper HTTP header parsing
	this->m_request_buffer.header_lf_to_crlf();

	// Check if we have received the complete HTTP header (ends with \r\n\r\n)
	if (this->m_request_buffer.is_header_finished())
	{
		// Extract the complete header from the request buffer
		std::string input = this->m_request_buffer.pull_header();
		
		// Parse HTTP request line and headers into structured data
		this->m_req_header.parse_request(input);
		
		// Extract connection type (keep-alive vs close) from headers
		this->m_connection_type = this->m_req_header.get_connection_type();
		
		// Move to next processing phase: target selection and routing
		this->m_process_state = PROCESS_SELECT_TARGET;
	}

	// Prevent header buffer overflow attacks - headers shouldn't exceed CHUNK_SIZE
	if (this->m_request_buffer.size() > CHUNK_SIZE)
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}
}

void Client::process_body()
{
	// Check if we have received the complete request body based on Content-Length
	if (this->m_request_buffer.size() >= this->m_req_header.get_content_length())
	{
		// Extract the request body data from buffer
		this->m_body = this->m_request_buffer.pull(this->m_req_header.get_content_length());
		
		// Body fully received, proceed to request processing
		this->m_process_state = PROCESS_REQUEST;
	}
	// If body is incomplete, we wait for more data in subsequent handle_read() calls
}

void Client::process_body_chunked_size()
{
	// Look for CRLF that terminates the chunk size line
	if (this->m_request_buffer.is_crlf_found())
	{
		// Extract the chunk size line (hex value + optional extensions)
		std::string chunk_size_str = this->m_request_buffer.pull_encoded();
		
		// Remove the trailing CRLF from buffer
		this->m_request_buffer.erase(2);
		
		// Parse hexadecimal chunk size into integer
		this->m_chunk_size = parse_chunk_size(chunk_size_str);
		
		// If chunk size > 0, prepare to read chunk data
		if (this->m_chunk_size)
		{
			// Accumulate total body size for validation against limits
			this->m_body_size += this->m_chunk_size;
			
			// Enforce client_max_body_size limit to prevent resource exhaustion
			if (this->m_body_size > this->m_target_block->get_client_max_body_size())
				throw WebservExceptions::HTTPException(HTTP_CONTENT_TOO_LARGE);
			
			// Move to chunk data reading phase
			this->m_process_state = PROCESS_BODY_CHUNKED_DATA;
		}
		else
		{
			// Chunk size is 0, indicating end of chunked transfer
			this->m_process_state = PROCESS_BODY_CHUNKED_END;
		}
	}
}

void Client::process_body_chunked_data()
{
	// Check if we have enough data: chunk size + trailing CRLF
	if (this->m_request_buffer.size() >= this->m_chunk_size + 2)
	{
		// Pull the chunk data plus its trailing CRLF
		std::string chunk_data = this->m_request_buffer.pull(this->m_chunk_size + 2);
		
		// Validate that chunk ends with proper CRLF
		if (chunk_data.compare(this->m_chunk_size, 2, "\r\n"))
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		
		// Remove the trailing CRLF from chunk data
		chunk_data.erase(this->m_chunk_size);
		
		// Append this chunk to the accumulated request body
		this->m_body.append(chunk_data);
		
		// Return to chunk size reading for next chunk
		this->m_process_state = PROCESS_BODY_CHUNKED_SIZE;
	}
}

void Client::process_body_chunked_end()
{
	// Wait for final CRLF that terminates chunked transfer
	if (this->m_request_buffer.size() >= 2)
	{
		// Pull the final CRLF sequence
		std::string crlf = this->m_request_buffer.pull(2);
		
		// Validate proper chunked transfer termination
		if (crlf.compare("\r\n"))
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		
		// Chunked transfer complete, proceed to request processing
		this->m_process_state = PROCESS_REQUEST;
	}
}

void Client::serve_autoindex(const std::deque<AutoIndexEntry>& entries)
{
	// Create HTML template for directory listing page
	std::string body = "<html>\n"
		"<head><title>Index of {template}</title></head>\n"
		"<body>\n"
		"<h1>Index of {template}</h1><hr><pre>\n";
	
	// Replace template placeholder with actual directory path
	replace_template_str(body, str_template, this->m_req_header.get_target());
	
	// Generate HTML entries for each file/directory in the listing
	for (size_t i = 0; i < entries.size(); i++)
	{
		const AutoIndexEntry& entry = entries[i];
		
		// Create clickable link for each entry
		std::string entry_html = "<a href=\"{template}\">{template}</a>";
		replace_template_str(entry_html, str_template, entry.ent_name);
		
		// For files (not directories), add timestamp and size information
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
	
	// Close HTML structure
	body.append(
		"</pre><hr></body>\n"
		"</html>\n"
	);

	// Generate complete HTTP response with autoindex content
	this->m_resp_header.set_content_length(body.size());
	this->m_resp_header.generate_response_fields(this->m_connection_type, HTTP_OK_MSG, false, "text/html");
	std::string response_header = this->m_resp_header.generate_response_header();
	
	// Push response header and body to output buffer
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
	
	// Reset client state for next request processing
	reset_client_state();
}

void Client::handle_index()
{
	// Attempt to find an appropriate index file for the requested directory
	IndexEntry index_entry = this->m_target_block->get_index_page(this->m_req_header.get_aug_target());
	
	// If the resolved index is actually a directory, redirect with trailing slash
	if (index_entry.is_dir)
	{
		const std::string& root = this->m_target_block->get_root();
		
		// Create relative path for redirect location
		std::string location = index_entry.path.substr(root.size());
		
		// Send 301 redirect to ensure proper directory URL format
		generate_error(HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG, location);
	}
	else
	{
		// Found a valid index file, prepare to serve it
		prep_process_file_body(index_entry.path);
	}
}

void Client::direct_serve(const BaseBlock* location_target)
{
	std::string& target = this->m_req_header.get_aug_target();
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

	int cgi_file_perm = R_OK;
	if (cgi_pass == "executable")
		cgi_file_perm = X_OK;
	if (stat(full_path.c_str(), &statbuf))
	{
		if (errno == ENOENT)
			throw WebservExceptions::CGINotFound();
		handle_http_file_errno();
	}
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	if (access(full_path.c_str(), cgi_file_perm))
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	if (cgi_pass != "executable")
	{
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
}

void Client::handle_cgi()
{
	const Location* location_target;
	std::string& target = this->m_req_header.get_aug_target();
	location_target = dynamic_cast<const Location*>(this->m_target_block);
	if (!location_target || !location_target->is_cgi_requirments(target))
		throw WebservExceptions::CGINotFound();
	std::string full_path = concat_path(location_target->get_root(), target);
	const std::string& cgi_pass = location_target->get_cgi_pass();
	validate_cgi_files_permissions(full_path, cgi_pass);
	this->m_script_name = this->m_req_header.get_target();
	this->m_cgi_handler.init_cgi(cgi_pass, full_path);
	this->m_process_state = PROCESS_CGI_BEGINNING;
}

void Client::handle_file_upload()
{
	const Location* location = dynamic_cast<const Location*>(this->m_target_block);
	if (!location)
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	std::string& target = this->m_req_header.get_aug_target();
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
	std::string& target = this->m_req_header.get_aug_target();
	std::string path = concat_path(this->m_target_block->get_root(), target);

	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
		handle_http_file_errno();
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::HTTPException(HTTP_IAM_A_TEAPOT);

	if (std::remove(path.c_str()))
		handle_http_file_errno();

	this->m_resp_header.generate_response_fields(HTTP_NO_CONTENT, HTTP_NO_CONTENT_MSG, false);
	std::string response_header = this->m_resp_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::process_request_get()
{
	std::string& target = this->m_req_header.get_aug_target();
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
			throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
	}
}

void Client::process_request_post()
{
	std::string& target = this->m_req_header.get_aug_target();
	std::string path = concat_path(this->m_target_block->get_root(), target);
	
	if (str_back(target) == '/')
		throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
	if (is_http_target_dir(path))
	{
		const std::string& root = this->m_target_block->get_root();
		std::string location = path.substr(root.size());
		generate_error(HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG, location);
	}
	else
	{
		try
		{
			handle_cgi();
		}
		catch (const WebservExceptions::CGINotFound& e)
		{
			handle_file_upload();
		}
	}
}

void Client::process_request_delete()
{
	std::string& target = this->m_req_header.get_aug_target();
	
	if (str_back(target) == '/')
		throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
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
		this->m_client_status = CLIENT_DONE;
	std::string& request_method = this->m_req_header.get_request_method();
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
	// Find the best matching server based on listen address and virtual host
	const Server* server = &this->m_server_container->get_best_server(
		this->m_server_addr->first, this->m_server_addr->second, this->m_req_header.get_virtual_host()
	);
	
	// Set server name for logging and error page generation
	if (server->get_server_names().size()) // Server has configured server names
		this->m_server_name = this->m_req_header.get_virtual_host();
	else
		this->m_server_name.clear(); // No server names configured

	// Initially target the server block
	this->m_target_block = server;
	
	// Reset augmented target to original request target for location matching
	this->m_req_header.set_aug_target(this->m_req_header.get_target());
	
	try
	{
		// Attempt to find the best matching location block within the server
		const Location* location = &server->match_location(this->m_req_header.get_target());
		this->m_target_block = location;
		
		// Validate that the HTTP method is allowed for this location
		if (!location->is_method_allowed(this->m_req_header.get_request_method()))
		{
			// Set Allow header with permitted methods for 405 response
			this->m_allowed_methods = &location->get_allowed_methods();
			throw WebservExceptions::HTTPException(HTTP_METHOD_NOT_ALLOWED);
		}
		
		// Adjust the augmented target by removing the location prefix
		std::string new_aug_target = this->m_req_header.get_aug_target();
		// Remove location path prefix but preserve the leading slash
		new_aug_target.erase(0, location->get_upload_path().size() - 1);
		
		this->m_req_header.set_aug_target(new_aug_target); // Update for further processing
		this->m_path_translated = this->m_req_header.get_target(); // Store original path
		this->m_path_translated.erase(0, location->get_upload_path().size());// remove upload path prefix
		if (this->m_path_translated.empty() || this->m_path_translated[0] != '/')// ensure leading slash
			this->m_path_translated.insert(this->m_path_translated.begin(), '/');// insert leading slash if missing
		this->m_path_translated = concat_path(location->get_root(), this->m_path_translated);// full translated path
	}
	catch (const WebservExceptions::LocationNotFound& e)
	{
	}
	
	this->m_document_root = this->m_target_block->get_root();// set document root
	if (this->m_req_header.get_content_length())// if there is a body
	{
		if (this->m_req_header.get_content_length() > this->m_target_block->get_client_max_body_size())// body too large
			throw WebservExceptions::HTTPException(HTTP_CONTENT_TOO_LARGE);
		this->m_process_state = PROCESS_BODY;// move to body processing state
	}
	else if (this->m_req_header.is_chunked())// if body is chunked
		this->m_process_state = PROCESS_BODY_CHUNKED_SIZE;// move to chunked size processing state
	else
		this->m_process_state = PROCESS_REQUEST;// move to request processing state
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
	this->m_resp_header.set_content_length(body.size());
	this->m_resp_header.generate_response_fields(this->m_connection_type, msg, false, "text/html");
	if (!location.empty())
		this->m_resp_header.add_field("Location", location);
	std::string response_header = this->m_resp_header.generate_response_header();
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	this->m_response_buffer.push(body.c_str(), body.size());
	this->m_response_buffer.create_barrier();
	reset_client_state();
}

void Client::generate_error(ushort code, const std::string& msg, const std::string& location)
{
	std::string error_page;

	this->m_resp_header.clear();
	if (this->m_allowed_methods)
		this->m_resp_header.set_allowed_methods(*this->m_allowed_methods);
	try
	{
		// Attempt to find a custom error page for this HTTP status code
		error_page = this->m_target_block->get_error_page(code);
		
		// If custom error page exists and is a file path (starts with '/')
		if (!error_page.empty() && error_page[0] == '/')
		{
			// Serve the custom error page file
			prep_process_file_body(error_page, msg);
		}
		else
		{
			// Error page is a redirect URL, generate redirect response
			fallback_generate_error(HTTP_FOUND_MSG, error_page);
		}
	}
	catch(const WebservExceptions::HTTPException& e)
	{
		// Error occurred while processing custom error page, use fallback
		fallback_generate_error(e.what(), location);
	}
	catch(const WebservExceptions::NoAvailablePage& e)
	{
		// No custom error page configured, use default error response
		fallback_generate_error(msg, location);
	}
}

void Client::prep_process_file_body(const std::string& file_path, const std::string& msg)
{
	struct stat statbuf;
	
	// Get file information (size, modification time, etc.)
	if (stat(file_path.c_str(), &statbuf))
		handle_http_file_errno(); // File doesn't exist or permission denied
	
	// Set Content-Length header based on file size
	this->m_resp_header.set_content_length(statbuf.st_size);
	
	// Open file for reading
	this->m_file_fd = open(file_path.c_str(), O_RDONLY);
	if (this->m_file_fd == -1)
		handle_http_file_errno(); // Failed to open file
	
	// Add file descriptor to poll set for non-blocking I/O
	this->m_server_container->add_to_poll(this->m_file_fd);
	
	// Determine appropriate MIME type based on file extension
	const char* media_type = get_media_type(file_path);
	
	// Set Last-Modified header for caching
	this->m_resp_header.set_last_modified(statbuf.st_mtim.tv_sec);
	
	// Generate HTTP response headers
	this->m_resp_header.generate_response_fields(this->m_connection_type, msg, false, media_type);
	std::string response_header = this->m_resp_header.generate_response_header();
	
	// Send response headers to client
	this->m_response_buffer.push(response_header.c_str(), response_header.size());
	
	// Switch to file body streaming state
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
			this->m_cgi_handler.write_cgi(chunk);
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
			this->m_cgi_buffer.header_lf_to_crlf();
			if (this->m_cgi_buffer.is_header_finished())
			{
				this->m_cgi_resp_header = this->m_cgi_buffer.pull_header();
				HTTPHeader tmp_header;
				tmp_header.set_chunked();
				tmp_header.parse_response(this->m_cgi_resp_header);
				this->m_resp_header.ignore_content_len_field();
				this->m_resp_header.generate_response_fields(
					this->m_connection_type, HTTP_OK_MSG, tmp_header.is_chunked(), "text/html"
				);
				this->m_resp_header.parse_response(this->m_cgi_resp_header);
				this->m_cgi_resp_header = this->m_resp_header.generate_response_header();
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
	if (this->m_cgi_handler.is_dead() && !this->m_body.empty())
	{
		this->m_cgi_handler.clean_handler();
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
	}
	if (this->m_cgi_header_finished
		&& (this->m_body.empty() || !this->m_cgi_handler.is_write_open()))
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
	size_t old_body_size = this->m_body_size;
	this->m_body_size += data.size();
	if (this->m_body_size > this->m_resp_header.get_content_length())
		data = data.substr(0, this->m_resp_header.get_content_length() - old_body_size);
	this->m_response_buffer.push(data.c_str(), data.size());
	if (this->m_body_size >= this->m_resp_header.get_content_length())
		reset_client_state();
}

void Client::process_cgi_read()
{
	if (!this->m_cgi_resp_header.empty())
	{
		this->m_response_buffer.push(this->m_cgi_resp_header.c_str(), this->m_cgi_resp_header.size());
		this->m_cgi_resp_header.clear();
	}
	if (this->m_cgi_buffer.size())
	{
		std::string data = this->m_cgi_buffer.pull(this->m_cgi_buffer.size());
		if (this->m_resp_header.is_chunked())
			handle_cgi_read_chunked(data);
		else
			handle_cgi_read(data);
	}
	if (!this->m_cgi_handler.is_read_open())
	{
		if (this->m_body_size < this->m_resp_header.get_content_length())
			throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
		reset_client_state();
	}
	else
	{
		if (this->m_cgi_handler.is_read_ready())
		{
			std::string data = this->m_cgi_handler.read_cgi();
			if (this->m_resp_header.is_chunked())
				handle_cgi_read_chunked(data);
			else
				handle_cgi_read(data);
		}
		if (this->m_cgi_handler.is_timeout())
			throw WebservExceptions::HTTPException(HTTP_GATEWAY_TIMEOUT);
	}
	this->m_cgi_handler.handle_death();
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
		std::string& target = this->m_req_header.get_aug_target();
		std::string path = concat_path(this->m_target_block->get_root(), target);
		std::string body = "File uploaded successfully to: " + path + "\n";
		this->m_resp_header.set_content_length(body.size());
		this->m_resp_header.add_field("Location", this->m_req_header.get_target());
		this->m_resp_header.generate_response_fields(HTTP_CREATED, HTTP_CREATED_MSG, false, "text/plain");
		std::string response_header = this->m_resp_header.generate_response_header();
		this->m_response_buffer.push(response_header.c_str(), response_header.size());
		this->m_response_buffer.push(body.c_str(), body.size());
		this->m_response_buffer.create_barrier();
		reset_client_state();
	}
}

void Client::process()
{
	// Only process if client is alive or in the process of finishing (CLIENT_DONE)
	if (this->m_client_status > CLIENT_DONE)
		return ;

	try
	{
		// State machine for HTTP request processing
		// Each state handles a specific phase of request/response cycle
		switch (this->m_process_state)
		{	
			case PROCESS_HEADER:
				// Parse HTTP request headers (only if client is still alive)
				if (this->m_client_status == CLIENT_ALIVE)
					process_header();
				break ;
				
			case PROCESS_SELECT_TARGET:
				// Route request to appropriate server/location block
				select_target();
				break ;
				
			case PROCESS_BODY:
				// Read request body using Content-Length
				process_body();
				break ;
				
			case PROCESS_BODY_CHUNKED_SIZE:
				// Parse chunk size in chunked transfer encoding
				process_body_chunked_size();
				break ;
				
			case PROCESS_BODY_CHUNKED_DATA:
				// Read chunk data in chunked transfer encoding
				process_body_chunked_data();
				break ;
				
			case PROCESS_BODY_CHUNKED_END:
				// Handle end of chunked transfer
				process_body_chunked_end();
				break ;
				
			case PROCESS_REQUEST:
				// Execute the actual HTTP request (GET/POST/DELETE)
				process_request();
				break ;
				
			case PROCESS_FILE_BODY:
				// Stream file content to client
				process_file_body();
				break ;
				
			case PROCESS_CGI_BEGINNING:
				// Initialize CGI script execution
				process_cgi_beginning();
				break ;
				
			case PROCESS_CGI_READ:
				// Read and process CGI script output
				process_cgi_read();
				break ;
				
			case PROCESS_FILE_UPLOAD:
				// Handle file upload processing
				process_file_upload();
				break ;
		}
	}
	catch (const WebservExceptions::HTTPException& e)
	{
		// Special handling for CGI errors: disconnect immediately
		if (this->m_process_state == PROCESS_CGI_READ)
		{
			this->m_client_status = CLIENT_DISCONNECTED;
			return ;
		}
		
		// For bad requests, clear buffer and force connection close
		if (e.get_error_code() == HTTP_BAD_REQUEST)
		{
			// Clear any remaining malformed data from buffer
			this->m_request_buffer.erase(this->m_request_buffer.size());
			this->m_client_status = CLIENT_DONE;
			this->m_connection_type = CONNECTION_CLOSE;
		}
		
		// Generate appropriate HTTP error response
		generate_error(e.get_error_code(), e.what(), "");
	}
}

void Client::reset_client_state()
{
	// Close any open file descriptors
	close_file();
	this->m_allowed_methods = 0;
	// Reset processing state to beginning of request cycle
	this->m_process_state = PROCESS_HEADER;
	
	// Reset target block to base server (before location matching)
	this->m_target_block = this->m_base_server;
	
	// Clear request-specific data
	this->m_body_size = 0;
	this->m_cgi_header_finished = false;
	
	// Clear HTTP headers and body data
	this->m_req_header.clear();
	this->m_resp_header.clear();
	this->m_body.clear();
	
	// Clear CGI-related buffers and handlers
	this->m_cgi_buffer.erase(this->m_cgi_buffer.size());
	this->m_cgi_handler.clean_handler();
}

void Client::close_file()
{
	// Close file descriptor if it's open
	if (this->m_file_fd != -1)
	{
		// Close the file descriptor
		close(this->m_file_fd);
		
		// Remove from poll set to prevent further monitoring
		this->m_server_container->remove_from_poll(this->m_file_fd);
		
		// Mark as closed
		this->m_file_fd = -1;
	}
}

time_t Client::get_last_activity() const
{
	return (this->m_last_activity);
}

HTTPHeader& Client::get_request_header()
{
	return (this->m_req_header);
}

const std::pair<std::string, std::string>& Client::get_client_addr() const
{
	return (this->m_client_addr);
}

const std::pair<std::string, std::string>& Client::get_server_addr() const
{
	return (*this->m_server_addr);
}

const std::string& Client::get_script_name()
{
	return (this->m_script_name);
}

const std::string& Client::get_server_name()
{
	return (this->m_server_name);
}

const std::string& Client::get_document_root()
{
	return (this->m_document_root);
}

const std::string& Client::get_path_translated()
{
	return (this->m_path_translated);
}

bool Client::is_client_completed()
{
	// Check if we've finished sending all response data
	// Client is marked DONE, response buffer is empty, and we're back to header processing
	if (this->m_client_status == CLIENT_DONE 
		&& !this->m_response_buffer.size()
		&& this->m_process_state == PROCESS_HEADER)
	{
		// All response data sent successfully, client can be disconnected
		this->m_client_status = CLIENT_DISCONNECTED;
		return true;
	}
	return false;
}