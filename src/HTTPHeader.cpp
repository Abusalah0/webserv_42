#include "../include/HTTPHeader.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include "../include/Exceptions.hpp"

HTTPHeader::HTTPHeader():
	m_is_query_paramaters(),
	m_is_chunked(),
	m_ignore_content_len_field(),
	m_connection(CONNECTION_KEEP_ALIVE),
	m_method(),
	m_content_len(),
	m_target(),
	m_aug_target(),
	m_query_parameters(),
	m_virtual_host(),
	m_response_msg(),
	m_fields(),
	m_response_fields(),
	m_allowed_methods(),
	m_last_modified()
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

	if (line.find(' ') == std::string::npos)// no spaces, so invalid eg. "GET/index.htmlHTTP/1.1"
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	while (line[i] != ' ')// find first space
		++i;
	if (!i)
	{
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	std::string method = line.substr(0, i);// extract method
	offset = i + 1;// set offset to next char after space

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

	if (line.size() - s_offset != 8)// "HTTP/x.x" is 8 chars
		return (false);
	// check for "HTTP/"
	if (line.compare(s_offset, 5, "HTTP/"))
		return (false);
	// check for major and minor version digits
	s_offset += 5;// move offset to major version char
	major_version = line[s_offset];// get major version char
	s_offset++;// move offset to '.'
	if (line[s_offset] != '.')// check for '.'
		return (false);
	s_offset++;// move offset to minor version char
	minor_version = line[s_offset];// get minor version char
	if (major_version != '1' || !std::isdigit(minor_version))// only HTTP/1.x supported
		return (false);
	// check for any extra chars after version
	if (s_offset + 1 < line.size())
		return (false);

	return (true);
}

bool validate_target(std::string& target)
{
	if (target[0] != '/')
		return (false);

	std::string component;
	size_t pos = 0;

	for (size_t i = 1; i < target.size(); i++)
	{
		if (target[i] == '/')
		{
			if (component == "..")
			{
				if (pos == 0)
					return (false);
				--pos;
			}
			else if (component != ".")
				pos++;
			component.clear();
		}
		else
			component += target[i];
	}
	if (component == ".." && !pos)
		return (false);
	return (true);
}

void HTTPHeader::parse_request_line(std::string& line)
{
	size_t s_offset = 0;
	size_t e_offset;
	// parse method eg. GET , POST , DELETE etc.
	// and set offset to next char after space
	this->m_method = request_parse_method(line, s_offset);
	e_offset = s_offset;

	while (e_offset < line.size())// find next space after method
	{
		if (line[e_offset] == ' ')
			break ;
		e_offset++;
	}

	if (e_offset == s_offset)// no target found
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	this->m_target = line.substr(s_offset, e_offset - s_offset);// extract target eg. /index.html
	size_t query_parameters_offset = this->m_target.find('?');// check for query parameters
	if (query_parameters_offset != std::string::npos)// if found, extract and set flag
	{
		this->m_is_query_paramaters = true;// set flag
		this->m_query_parameters = this->m_target.substr(query_parameters_offset + 1);// extract query parameters
		this->m_target.erase(query_parameters_offset);// remove query parameters from target
	}
	if (!validate_target(this->m_target))// validate target
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	// normalize target path (eg. /a/b/../c -> /a/c)
	std::string target = normalize_path(this->m_target);
	// preserve trailing slash if present in original target
	if (str_back(target) != '/' && str_back(this->m_target) == '/')
		target.push_back('/');

	this->m_target = target;// set normalized target
	this->m_target = url_decode(this->m_target);// URL decode target eg. /index%20page.html -> /index page.html
	e_offset++;
	s_offset = e_offset;

	if (!validate_http_version(line, s_offset))// check for valid HTTP version
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
}

/**
 * @brief Splits a header line into its name and value components.
 * @param line The header line to split.
 * @return An HTTPHeaderField struct containing the name and value.
 * @throws WebservExceptions::BadRequest on invalid input.
 */
static HTTPHeaderField split_request_header(std::string& line)
{
	HTTPHeaderField field;
	size_t s_offset = 0;
	size_t e_offset = 0;

	// find colon separating name and value
	while (e_offset < line.size())
	{
		if (line[e_offset] == ':')
			break ;
		e_offset++;
	}
	if (!e_offset || e_offset == line.size())// no colon or empty name
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	field.name = line.substr(s_offset, e_offset);// extract name
	s_offset = e_offset + 1;// move offset to next char after colon
	if (s_offset == line.size())// no value
		return (field);

	while (s_offset < line.size())// skip leading whitespace in value
	{
		if (!is_ws_chr(line[s_offset]))// non-whitespace char found
			break ;
		s_offset++;
	}
	if (s_offset == line.size())// no value
		return (field);

	e_offset = line.size() - 1;
	while (e_offset > s_offset)// trim trailing whitespace in value
	{
		if (!is_ws_chr(line[e_offset]))
			break ;
		e_offset--;
	}
	// extract value without leading/trailing whitespace eg. "  text/html  " -> "text/html"
	field.value = line.substr(s_offset, (e_offset - s_offset + 1));
	return (field);
}

HTTPHeaderField split_response_header(std::string& line)
{
	HTTPHeaderField field;
	size_t s_offset = 0;
	size_t e_offset = 0;

	while (e_offset < line.size())// find colon separating name and value
	{
		if (line[e_offset] == ':')
			break ;
		e_offset++;
	}
	if (!e_offset || e_offset == line.size())
		throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);

	field.name = line.substr(s_offset, e_offset);// extract name
	s_offset = e_offset + 1;
	if (s_offset == line.size())
		return (field);

	while (s_offset < line.size())// skip leading whitespace in value
	{
		if (!is_ws_chr(line[s_offset]))
			break ;
		s_offset++;
	}
	if (s_offset == line.size())
		return (field);

	e_offset = line.size() - 1;
	while (e_offset > s_offset)// trim trailing whitespace in value
	{
		if (!is_ws_chr(line[e_offset]))
			break ;
		e_offset--;
	}
	// extract value without leading/trailing whitespace eg. "  text/html  " -> "text/html"
	field.value = line.substr(s_offset, (e_offset - s_offset + 1));
	return (field);
}

char c_tolower(char c)
{
	if (c >= 0x41 && c <= 0x5A)
		return c + 0x20;
	return c;
}

void HTTPHeader::parse_request_header_line(std::string& line)
{
	HTTPHeaderField field = split_request_header(line);

	if (!check_str_chrs(field.name, is_token_chr))// check for valid token characters
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	if (!check_str_chrs(field.value, is_field_value_chr))// check for valid field value characters
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	std::string lowercase_name = field.name;
	// convert name to lowercase for case-insensitive comparison
	std::transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), c_tolower);

	if (this->m_fields.find(lowercase_name) != this->m_fields.end())// if field already exists, concatenate values for certain headers
	{
		if (lowercase_name == "content-length")// content-length must not appear multiple times
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
		if (lowercase_name == "cookie")// cookies are separated by '; '
			this->m_fields[lowercase_name].value.append("; ");
		else
			this->m_fields[lowercase_name].value.append(", ");
		this->m_fields[lowercase_name].value.append(field.value);// concatenate values
	}
	else
		this->m_fields[lowercase_name] = field;
}

void HTTPHeader::parse_content_len()
{
	char *endptr;

	std::string& value = this->m_fields["content-length"].value;// get content length value
	if (value.empty() || !std::isdigit(value[0]))// invalid content length
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	errno = 0;
	this->m_content_len = strtoul(value.c_str(), &endptr, 10);// convert to unsigned long
	if (errno == ERANGE || endptr != value.c_str() + value.size())// conversion error or out of range
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
}

void HTTPHeader::parse_transfer_encoding()
{
	if (this->m_fields.find("content-length") != this->m_fields.end())// both content-length and transfer-encoding present
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);

	std::string temp_value = this->m_fields["transfer-encoding"].value;// get transfer-encoding value
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);// to lowercase
	if (temp_value != "chunked")// only chunked encoding supported
		throw WebservExceptions::HTTPException(HTTP_NOT_IMPLEMENTED);

	this->m_is_chunked = true;// set chunked flag
}

void HTTPHeader::parse_connection()
{
	std::string temp_value = this->m_fields["connection"].value;// get connection value

	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);// to lowercase
	if (temp_value.find("close") != std::string::npos)// if "close" found, set connection to close
		this->m_connection = CONNECTION_CLOSE;// set connection to close
}

void HTTPHeader::parse_request(std::string& input)
{
	size_t line_start = 0;
	std::string line;

	while (true)
	{
		size_t crlf_pos = input.find("\r\n", line_start);// find next CRLF
		if (crlf_pos == line_start)
			break ;
		line = input.substr(line_start, crlf_pos - line_start);// extract line till CRLF

		if (!line_start)// first line
			parse_request_line(line);// parse request line (eg. GET /index.html HTTP/1.1)
		else
			parse_request_header_line(line);// parse header line (eg. Host: example.com)

		line_start = crlf_pos + 2;// move to next line
	}

	if (this->m_fields.find("host") == this->m_fields.end())// Host field is mandatory in HTTP/1.1
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	else
	{
		this->m_virtual_host = this->m_fields["host"].value;// get virtual host
		if (this->m_virtual_host.empty())// empty host value is invalid
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	}

	if (this->m_fields.find("content-length") != this->m_fields.end())// parse content length if present
		parse_content_len();
	if (this->m_fields.find("transfer-encoding") != this->m_fields.end())// parse transfer encoding if present
		parse_transfer_encoding();
	if (this->m_fields.find("connection") != this->m_fields.end())// parse connection if present
		parse_connection();
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

void HTTPHeader::generate_response_fields(int client_status,
	const std::string& msg,
	bool is_chunked,
	const char* media_type)
{
	add_field("Server", SERVER_SOFTWARE);
	if (is_chunked)
		add_field("Transfer-Encoding", "chunked");
	else if (!this->m_ignore_content_len_field)
		add_field("Content-Length", ul_to_str(this->m_content_len));
	std::string conn_value = "close";
	if (!client_status)
		conn_value = "keep-alive";
	add_field("Connection", conn_value);
	if (*media_type)
		add_field("Content-Type", media_type);
	if (!this->m_last_modified.empty())
		add_field("Last-Modified", this->m_last_modified);
	if (!this->m_allowed_methods.empty())
		add_field("Allow", this->m_allowed_methods);
	add_field("Date", generate_http_date());

	this->m_is_chunked = true;
	if (this->m_response_msg.empty())
		this->m_response_msg = msg;
}

std::string HTTPHeader::generate_response_header()
{
	std::string res;
	res.append("HTTP/1.1 ");
	res.append(this->m_response_msg);
	res.append("\r\n");
	for (std::map<std::string, HTTPHeaderField>::iterator it = this->m_fields.begin();
		it != this->m_fields.end(); it++)
	{
		HTTPHeaderField& field = (*it).second;
		res.append(field.name);
		res.append(": ");
		res.append(field.value);
		res.append("\r\n");
	}
	for (size_t i = 0; i < this->m_response_fields.size(); i++)
	{
		HTTPHeaderField& field = this->m_response_fields[i];
		res.append(field.name);
		res.append(": ");
		res.append(field.value);
		res.append("\r\n");
	}
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
