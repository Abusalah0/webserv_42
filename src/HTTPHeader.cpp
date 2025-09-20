/**
 * @file HTTPHeader.cpp
 * @brief Implementation of HTTP header parsing and generation functionality.
 * 
 * This file implements comprehensive HTTP/1.1 header processing including:
 * - Request line parsing with method, target, and version validation
 * - Header field parsing with proper RFC 7230 compliance
 * - Query parameter extraction and URL decoding
 * - Transfer encoding detection (chunked vs content-length)
 * - Virtual host resolution for multi-domain serving
 * - Response header generation with proper formatting
 * - Connection management (keep-alive vs close)
 * - Path normalization and security validation
 */

#include "../include/HTTPHeader.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include "../include/Exceptions.hpp"

HTTPHeader::HTTPHeader():
	m_is_query_paramaters(false),           // No query parameters initially
	m_is_chunked(false),                    // Default to non-chunked transfer
	m_ignore_content_len_field(false),     // Process Content-Length by default
	m_connection(CONNECTION_KEEP_ALIVE),    // Default to keep-alive connections
	m_method(),                             // HTTP method (GET, POST, etc.)
	m_content_len(0),                       // Content length starts at 0
	m_target(),                             // Request target URI
	m_aug_target(),                         // Augmented target for processing
	m_query_parameters(),                   // Query string parameters
	m_virtual_host(),                       // Host header value
	m_response_msg(),                       // HTTP status message
	m_fields(),                             // Header fields map
	m_response_fields(),                    // Response headers queue
	m_allowed_methods(),                    // Allowed methods for 405 responses
	m_last_modified()                       // Last-Modified header value
{}

HTTPHeader::~HTTPHeader()
{}

/**
 * @brief Parses the HTTP method from the request line.
 * @param line The request line containing the HTTP method.
 * @param offset Reference to size_t to store the offset after the method.
 * @return The parsed HTTP method as a string.
 * @throws WebservExceptions::BadRequest on invalid input.
 * @throws std::bad_alloc on allocation failure.
 */
static std::string request_parse_method(std::string& line, size_t& offset)
{
	size_t i = 0;

	// Validate that request line contains spaces (required format: "METHOD URI HTTP/1.1")
	if (line.find(' ') == std::string::npos)
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	// Find the first space that terminates the HTTP method
	while (line[i] != ' ')
		++i;
	
	// Method cannot be empty
	if (!i)
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	// Extract HTTP method (GET, POST, DELETE, etc.)
	std::string method = line.substr(0, i);
	
	// Set offset to character after the space for continued parsing
	offset = i + 1;

	return (method);
}

/**
 * @brief Validates the HTTP version in the request line.
 * @param line The request line containing the HTTP version.
 * @param s_offset The starting offset of the HTTP version in the line.
 * @return true if the HTTP version is valid, false otherwise.
 */
static bool validate_http_version(std::string& line, size_t s_offset)
{
	char major_version;
	char minor_version;

	// HTTP version must be exactly 8 characters: "HTTP/x.x"
	if (line.size() - s_offset != 8)
		return (false);
	
	// Validate "HTTP/" prefix
	if (line.compare(s_offset, 5, "HTTP/"))
		return (false);
	
	// Extract version digits
	s_offset += 5; // Move to major version digit
	major_version = line[s_offset];
	
	s_offset++; // Move to dot separator
	if (line[s_offset] != '.')
		return (false);
	
	s_offset++; // Move to minor version digit  
	minor_version = line[s_offset];
	
	// Only HTTP/1.x versions are supported (1.0, 1.1)
	if (major_version != '1' || !std::isdigit(minor_version))
		return (false);
	
	// Ensure no trailing characters after version
	if (s_offset + 1 < line.size())
		return (false);

	return (true);
}

/**
 * @brief Validates HTTP request target for security and format compliance.
 * Prevents path traversal attacks by checking for invalid ".." sequences
 * that could escape the document root directory.
 * @param target The request target URI to validate
 * @return true if target is safe and valid, false otherwise
 */
bool validate_target(std::string& target)
{
	// Request target must start with '/' (absolute path)
	if (target[0] != '/')
		return (false);

	std::string component;
	size_t pos = 0; // Current directory depth

	// Parse each path component separated by '/'
	for (size_t i = 1; i < target.size(); i++)
	{
		if (target[i] == '/')
		{
			// Handle parent directory reference
			if (component == "..")
			{
				// Cannot go above root directory
				if (pos == 0)
					return (false);
				--pos; // Move up one directory level
			}
			// Ignore current directory references, count normal directories
			else if (component != ".")
				pos++;
			
			component.clear(); // Reset for next component
		}
		else
			component += target[i]; // Build current path component
	}
	
	// Check final component for path traversal attempt at root level
	if (component == ".." && !pos)
		return (false);
	
	return (true);
}

/**
 * @brief Parses the HTTP request line (first line of HTTP request).
 * Extracts and validates the method, target URI, and HTTP version.
 * Format: "METHOD /target HTTP/version" with optional query parameters.
 * @param line The complete request line string to parse
 * @throws HTTPException with HTTP_BAD_REQUEST for invalid format or security violations
 */
void HTTPHeader::parse_request_line(std::string& line)
{
	size_t s_offset = 0;
	size_t e_offset;
	
	// Parse HTTP method (GET, POST, DELETE) and advance offset past method
	this->m_method = request_parse_method(line, s_offset);
	e_offset = s_offset;

	// Find the space delimiter after the method
	while (e_offset < line.size())
	{
		if (line[e_offset] == ' ')
			break ;
		e_offset++;
	}

	// Ensure target exists after method
	if (e_offset == s_offset)
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	// Extract the request target (URI path with optional query string)
	this->m_target = line.substr(s_offset, e_offset - s_offset);
	
	// Check for query parameters separated by '?'
	size_t query_parameters_offset = this->m_target.find('?');
	if (query_parameters_offset != std::string::npos)
	{
		this->m_is_query_paramaters = true;
		// Extract query parameters (everything after '?')
		this->m_query_parameters = this->m_target.substr(query_parameters_offset + 1);
		// Remove query parameters from target path
		this->m_target.erase(query_parameters_offset);
	}
	
	// Validate target for security (prevent path traversal attacks)
	if (!validate_target(this->m_target))
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		
	// Normalize target path (resolve '..' and '.' components)
	std::string target = normalize_path(this->m_target);
	
	// Preserve trailing slash semantics for directory requests
	if (str_back(target) != '/' && str_back(this->m_target) == '/')
		target.push_back('/');

	this->m_target = target;
	// URL decode percent-encoded characters (e.g., %20 -> space)
	this->m_target = url_decode(this->m_target);
	
	// Move to HTTP version part of request line
	e_offset++;
	s_offset = e_offset;

	// Validate HTTP version format and support
	if (!validate_http_version(line, s_offset))
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
}

/**
 * @brief Splits a header line into its name and value components.
 * Parses HTTP header field format: "Header-Name: Header-Value"
 * Handles whitespace trimming and validates header field structure.
 * @param line The header line to split (modified in place for efficiency)
 * @return An HTTPHeaderField struct containing the name and value
 * @throws WebservExceptions::BadRequest on invalid header format
 */
static HTTPHeaderField split_request_header(std::string& line)
{
	HTTPHeaderField field;
	size_t s_offset = 0;
	size_t e_offset = 0;

	// Find colon separator between header name and value
	while (e_offset < line.size())
	{
		if (line[e_offset] == ':')
			break ;
		e_offset++;
	}
	
	// Validate header format: must have colon and non-empty name
	if (!e_offset || e_offset == line.size())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	// Extract header name (everything before colon)
	field.name = line.substr(s_offset, e_offset);
	
	// Move to start of header value (after colon)
	s_offset = e_offset + 1;
	if (s_offset == line.size()) // Header with no value
		return (field);

	// Skip leading whitespace in header value
	while (s_offset < line.size())
	{
		if (!is_ws_chr(line[s_offset]))
			break ;
		s_offset++;
	}
	if (s_offset == line.size()) // Only whitespace after colon
		return (field);

	// Trim trailing whitespace from header value
	e_offset = line.size() - 1;
	while (e_offset > s_offset)
	{
		if (!is_ws_chr(line[e_offset]))
			break ;
		e_offset--;
	}
	
	// Extract trimmed header value (e.g., "  text/html  " -> "text/html")
	field.value = line.substr(s_offset, (e_offset - s_offset + 1));
	return (field);
}

/**
 * @brief Splits a response header line into its name and value components.
 * Similar to split_request_header but for CGI response headers.
 * Parses HTTP header field format: "Header-Name: Header-Value"
 * @param line The header line to split (modified in place for efficiency)
 * @return An HTTPHeaderField struct containing the name and value
 * @throws WebservExceptions::HTTPException with HTTP_BAD_GATEWAY on invalid format
 */
HTTPHeaderField split_response_header(std::string& line)
{
	HTTPHeaderField field;
	size_t s_offset = 0;
	size_t e_offset = 0;

	// Find colon separator between header name and value
	while (e_offset < line.size())
	{
		if (line[e_offset] == ':')
			break ;
		e_offset++;
	}
	
	// Validate header format for CGI response
	if (!e_offset || e_offset == line.size())
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);

	// Extract header name (everything before colon)
	field.name = line.substr(s_offset, e_offset);
	s_offset = e_offset + 1;
	if (s_offset == line.size())
		return (field);

	// Skip leading whitespace in header value
	while (s_offset < line.size())
	{
		if (!is_ws_chr(line[s_offset]))
			break ;
		s_offset++;
	}
	if (s_offset == line.size())
		return (field);

	// Trim trailing whitespace from header value
	e_offset = line.size() - 1;
	while (e_offset > s_offset)
	{
		if (!is_ws_chr(line[e_offset]))
			break ;
		e_offset--;
	}
	
	// Extract trimmed header value (e.g., "  text/html  " -> "text/html")
	field.value = line.substr(s_offset, (e_offset - s_offset + 1));
	return (field);
}

char c_tolower(char c)
{
	if (c >= 0x41 && c <= 0x5A)
		return c + 0x20;
	return c;
}

/**
 * @brief Parses a single HTTP header field line and adds it to the header collection.
 * Validates field name and value format, handles duplicate headers according to HTTP spec.
 * Header names are normalized to lowercase for case-insensitive comparison.
 * @param line The header line to parse (e.g., "Content-Type: text/html")
 * @throws HTTPException with HTTP_BAD_REQUEST for invalid header format or characters
 */
void HTTPHeader::parse_request_header_line(std::string& line)
{
	HTTPHeaderField field = split_request_header(line);

	// Validate header name contains only valid token characters
	if (!check_str_chrs(field.name, is_token_chr))
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	
	// Validate header value contains only valid field value characters
	if (!check_str_chrs(field.value, is_field_value_chr))
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	// Normalize header name to lowercase for case-insensitive handling
	std::string lowercase_name = field.name;
	std::transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), c_tolower);

	// Handle duplicate headers according to HTTP specification
	if (this->m_fields.find(lowercase_name) != this->m_fields.end())
	{
		// Content-Length must appear only once (RFC 7230)
		if (lowercase_name == "content-length")
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
			
		// Cookie headers use semicolon separator
		if (lowercase_name == "cookie")
			this->m_fields[lowercase_name].value.append("; ");
		else
			this->m_fields[lowercase_name].value.append(", "); // Standard comma separator
			
		this->m_fields[lowercase_name].value.append(field.value);
	}
	else
		this->m_fields[lowercase_name] = field; // First occurrence of this header
}

/**
 * @brief Parses the Content-Length header field and sets the content length.
 * Validates that the value is a valid non-negative integer and converts it to size_t.
 * Content-Length indicates the size of the request body in bytes.
 * @throws HTTPException with HTTP_BAD_REQUEST for invalid or out-of-range values
 */
void HTTPHeader::parse_content_len()
{
	char *endptr;

	// Get Content-Length header value
	std::string& value = this->m_fields["content-length"].value;
	if (value.empty() || !std::isdigit(value[0])) // Must start with digit
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	// Convert string to unsigned long with error checking
	errno = 0;
	this->m_content_len = strtoul(value.c_str(), &endptr, 10);
	
	// Check for conversion errors or trailing characters
	if (errno == ERANGE || endptr != value.c_str() + value.size())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
}

/**
 * @brief Parses the Transfer-Encoding header field and configures encoding mode.
 * HTTP/1.1 prohibits both Content-Length and Transfer-Encoding in the same message.
 * Only "chunked" transfer encoding is supported by this server implementation.
 * @throws HTTPException with HTTP_BAD_REQUEST if both Content-Length and Transfer-Encoding present
 * @throws HTTPException with HTTP_NOT_IMPLEMENTED for unsupported encoding types
 */
void HTTPHeader::parse_transfer_encoding()
{
	// RFC 7230: Content-Length and Transfer-Encoding are mutually exclusive
	if (this->m_fields.find("content-length") != this->m_fields.end())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	// Get transfer-encoding value and normalize to lowercase
	std::string temp_value = this->m_fields["transfer-encoding"].value;
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);
	
	// Only chunked encoding is supported in this implementation
	if (temp_value != "chunked")
		throw WebservExceptions::HTTPException(HTTP_NOT_IMPLEMENTED);

	this->m_is_chunked = true; // Enable chunked transfer processing
}

/**
 * @brief Parses the Connection header field and sets connection handling mode.
 * Determines whether the connection should be kept alive or closed after response.
 * Default behavior is keep-alive unless "close" is explicitly specified.
 */
void HTTPHeader::parse_connection()
{
	// Get Connection header value and normalize to lowercase
	std::string temp_value = this->m_fields["connection"].value;
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);
	
	// Check if client requests connection closure
	if (temp_value.find("close") != std::string::npos)
		this->m_connection = CONNECTION_CLOSE;
	// Default is keep-alive (CONNECTION_KEEP_ALIVE)
}

/**
 * @brief Parses a complete HTTP request message into its components.
 * Processes the request line and all header fields, then validates
 * critical fields like Host, Content-Length, Transfer-Encoding, and Connection.
 * @param input The complete HTTP request string including headers
 * @throws HTTPException with HTTP_BAD_REQUEST for invalid requests or missing Host header
 */
void HTTPHeader::parse_request(std::string& input)
{
	size_t line_start = 0;
	std::string line;

	// Parse each line of the HTTP request
	while (true)
	{
		// Find end of current line (CRLF sequence)
		size_t crlf_pos = input.find("\r\n", line_start);
		if (crlf_pos == line_start) // Empty line indicates end of headers
			break ;
			
		// Extract current line
		line = input.substr(line_start, crlf_pos - line_start);

		if (!line_start) // First line is the request line
			parse_request_line(line); // Parse "GET /path HTTP/1.1"
		else
			parse_request_header_line(line); // Parse "Header-Name: value"

		line_start = crlf_pos + 2; // Move past CRLF to next line
	}

	// Validate mandatory Host header (required in HTTP/1.1)
	if (this->m_fields.find("host") == this->m_fields.end())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	else
	{
		// Extract virtual host from Host header
		this->m_virtual_host = this->m_fields["host"].value;
		if (this->m_virtual_host.empty()) // Empty host value is invalid
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	// Parse optional headers that affect request processing
	if (this->m_fields.find("content-length") != this->m_fields.end())
		parse_content_len(); // Parse body size for POST requests
	if (this->m_fields.find("transfer-encoding") != this->m_fields.end())
		parse_transfer_encoding(); // Check for chunked encoding
	if (this->m_fields.find("connection") != this->m_fields.end())
		parse_connection(); // Check for keep-alive or close
}

void HTTPHeader::parse_response_content_len()
{
	char *endptr;
	// get content length value
	std::string& value = this->m_fields["content-length"].value;
	if (value.empty() || !std::isdigit(value[0]))
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);

	errno = 0;
	// convert to unsigned long
	this->m_content_len = strtoul(value.c_str(), &endptr, 10);
	if (errno == ERANGE || endptr != value.c_str() + value.size())// conversion error or out of range
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
}

void HTTPHeader::parse_response_header_line(std::string& line)
{
	HTTPHeaderField field = split_response_header(line);
	// validate field name and value
	if (!check_str_chrs(field.name, is_token_chr))
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
	if (!check_str_chrs(field.value, is_field_value_chr))
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);

	std::string lowercase_name = field.name;
	// convert name to lowercase for case-insensitive comparison
	std::transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), c_tolower);
	if (this->m_fields.find(lowercase_name) != this->m_fields.end())
	{
		if (lowercase_name == "content-length")// content-length must not appear multiple times
			throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
		this->m_fields[lowercase_name] = field;
	}
	else if (lowercase_name == "content-length")// parse content length
	{
		this->m_is_chunked = false;// reset chunked flag if content-length present
		this->m_fields[lowercase_name] = field;// add field
		parse_response_content_len();// parse content length
	}
	else if (lowercase_name == "status")// parse status message
	{
		if (!is_response_status_valid(field.value))
			throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
		this->m_response_msg = field.value;
	}
	else
		this->m_response_fields.push_back(field);
}

void HTTPHeader::parse_response(std::string& input)
{
	size_t line_start = 0;
	std::string line;
	while (true)
	{
		size_t crlf_pos = input.find("\r\n", line_start);
		if (crlf_pos == line_start)
			break;
		line = input.substr(line_start, crlf_pos - line_start);
		parse_response_header_line(line);
		line_start = crlf_pos + 2;
	}
}

/**
 * @brief Generates HTTP response header fields based on response parameters.
 * Sets standard response headers including Server, Transfer-Encoding/Content-Length,
 * Connection, Content-Type, Last-Modified, Allow, and Date headers.
 * @param client_status Client status code determining connection behavior (0 = keep-alive)
 * @param msg HTTP status message for the response
 * @param is_chunked Whether to use chunked transfer encoding
 * @param media_type MIME type for Content-Type header (empty string to omit)
 */
void HTTPHeader::generate_response_fields(int client_status,
	const std::string& msg,
	bool is_chunked,
	const char* media_type)
{
	// Add server identification header
	add_field("Server", SERVER_SOFTWARE);
	
	// Set body transfer method (chunked vs content-length)
	if (is_chunked)
		add_field("Transfer-Encoding", "chunked");
	else if (!this->m_ignore_content_len_field)
		add_field("Content-Length", ul_to_str(this->m_content_len));
	
	// Set connection handling based on client status
	std::string conn_value = "close";
	if (!client_status) // Status 0 indicates healthy connection
		conn_value = "keep-alive";
	add_field("Connection", conn_value);
	
	// Add optional headers if values are provided
	if (*media_type) // Content-Type for response body
		add_field("Content-Type", media_type);
	if (!this->m_last_modified.empty()) // File modification time
		add_field("Last-Modified", this->m_last_modified);
	if (!this->m_allowed_methods.empty()) // Allowed methods for 405 responses
		add_field("Allow", this->m_allowed_methods);
	
	// Add current timestamp
	add_field("Date", generate_http_date());

	// Configure response settings
	this->m_is_chunked = true;
	if (this->m_response_msg.empty())
		this->m_response_msg = msg;
}

/**
 * @brief Generates the complete HTTP response header as a formatted string.
 * Combines the status line and all header fields into proper HTTP format
 * with CRLF line endings and header termination.
 * @return Complete HTTP response header string ready for transmission
 */
std::string HTTPHeader::generate_response_header()
{
	std::string res;
	
	// Build HTTP status line (e.g., "HTTP/1.1 200 OK")
	res.append("HTTP/1.1 ");
	res.append(this->m_response_msg);
	res.append("\r\n");
	
	// Add parsed request headers (used in CGI processing)
	for (std::map<std::string, HTTPHeaderField>::iterator it = this->m_fields.begin();
		it != this->m_fields.end(); it++)
	{
		HTTPHeaderField& field = (*it).second;
		res.append(field.name);
		res.append(": ");
		res.append(field.value);
		res.append("\r\n");
	}
	
	// Add response-specific headers generated by server
	for (size_t i = 0; i < this->m_response_fields.size(); i++)
	{
		HTTPHeaderField& field = this->m_response_fields[i];
		res.append(field.name);
		res.append(": ");
		res.append(field.value);
		res.append("\r\n");
	}
	
	// Add empty line to terminate headers (RFC 7230)
	res.append("\r\n");
	return res;
}

bool HTTPHeader::is_query_parameters()
{
	return (this->m_is_query_paramaters);
}

bool HTTPHeader::is_chunked()
{
	return (this->m_is_chunked);
}

ConnectionTypes HTTPHeader::get_connection_type()
{
	return (this->m_connection);
}

std::string& HTTPHeader::get_request_method()
{
	return (this->m_method);
}

size_t HTTPHeader::get_content_length()
{
	return (this->m_content_len);
}

void HTTPHeader::set_content_length(size_t len)
{
	this->m_content_len = len;
}

std::string& HTTPHeader::get_target()
{
	return (this->m_target);
}

std::string& HTTPHeader::get_query_parameters()
{
	return (this->m_query_parameters);
}

std::string& HTTPHeader::get_virtual_host()
{
	return (this->m_virtual_host);
}

std::map<std::string, HTTPHeaderField>& HTTPHeader::get_fields()
{
	return (this->m_fields);
}

std::deque<HTTPHeaderField>& HTTPHeader::get_response_fields()
{
	return (this->m_response_fields);
}

void HTTPHeader::add_field(const std::string& name, const std::string& value)
{
	HTTPHeaderField field;
	std::string lowercase = name;
	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), c_tolower);
	field.name = name;
	field.value = value;
	this->m_fields[lowercase] = field;
}

void HTTPHeader::clear()
{
	this->m_is_query_paramaters = false;
	this->m_is_chunked = false;
	this->m_ignore_content_len_field = false;
	this->m_connection = CONNECTION_KEEP_ALIVE;
	this->m_content_len = 0;
	this->m_query_parameters.clear();
	this->m_response_msg.clear();
	this->m_fields.clear();
	this->m_response_fields.clear();
	this->m_allowed_methods.clear();
	this->m_last_modified.clear();
}

void HTTPHeader::debug()
{
	std::cout << "Method: " << this->m_method << std::endl;
	std::cout << "Target: " << this->m_target << std::endl;
	std::cout << "Is Chunked: " << (this->m_is_chunked ? "Yes" : "No") << std::endl;
	std::cout << "Connection: " << (this->m_connection ? "close" : "keep-alive") << std::endl;
	std::cout << "Content-Length: " << this->m_content_len << std::endl;
	if (this->m_is_query_paramaters)
		std::cout << "Query Parameters: " << this->m_query_parameters << std::endl;
	std::cout << "---------Fields---------" << std::endl;
	for (std::map<std::string, HTTPHeaderField>::iterator it = this->m_fields.begin();
		it != this->m_fields.end(); it++)
	{
		HTTPHeaderField entry = (*it).second;
		std::cout << entry.name << ": " << entry.value << std::endl;
	}
}

void HTTPHeader::set_chunked()
{
	this->m_is_chunked = true;
}

void HTTPHeader::ignore_content_len_field()
{
	this->m_ignore_content_len_field = true;
}

std::string& HTTPHeader::get_aug_target()
{
	return (this->m_aug_target);
}

void HTTPHeader::set_aug_target(const std::string& str)
{
	this->m_aug_target = str;
}

void HTTPHeader::set_last_modified(time_t raw_time)
{
	this->m_last_modified = generate_http_date(raw_time);
}

void HTTPHeader::set_allowed_methods(const std::set<std::string>& allowed_methods)
{
	std::set<std::string>::iterator it = allowed_methods.begin();

	while (it != allowed_methods.end())
	{
		this->m_allowed_methods.append(*it);
		it++;
		if (it != allowed_methods.end())
			this->m_allowed_methods.append(", ");
	}
}
