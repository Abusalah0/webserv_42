#include "../include/RequestHeader.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

RequestHeader::RequestHeader():
	m_is_query_paramaters(),
	m_is_chunked(),
	m_connection(CONNECTION_KEEP_ALIVE),
	m_method(),
	m_content_len(),
	m_target(),
	m_query_parameters(),
	m_virtual_host(),
	m_fields()
{}

RequestHeader::~RequestHeader()
{}

bool is_token_chr(u_char c)
{
	if (c != '!' && c != '#' && c != '$'
		&& c != '%' && c != '&' && c != '\''
		&& c != '*' && c != '+' && c != '-'
		&& c != '.' && c != '^' && c != '_'
		&& c != '`' && c != '|' && c != '~'
		&& !std::isalnum(c))
		return false;
	return true;
}

bool is_ws_chr(u_char c)
{
	if (c == ' ' || c == '\t')
		return true;
	return false;
}

bool is_vchar(u_char c)
{
	if (c >= 0x21 && c <= 0x7E)
		return true;
	return false;
}

bool is_obs_chr(u_char c)
{
	if (c >= 0x80)
		return true;
	return false;
}

bool is_field_value_chr(u_char c)
{
	if (!is_ws_chr(c) && !is_vchar(c) && !is_obs_chr(c))
		return false;
	return true;
}


bool check_str_chrs(std::string& str, bool (*func)(u_char c))
{
	size_t i = 0;
	while (i < str.size())
	{
		if (!func(str[i]))
			return false;
		i++;
	}
	return true;
}

RequestMethods request_parse_method(std::string& line, size_t& offset)
{
	if (!line.compare(0, 4, "GET "))
	{
		offset = 4;
		return GET_METHOD;
	}
	if (!line.compare(0, 5, "POST "))
	{
		offset = 5;
		return POST_METHOD;
	}
	if (!line.compare(0, 7, "DELETE "))
	{
		offset = 7;
		return DELETE_METHOD;
	}
	throw WebservExceptions::BadRequest();
}

bool validate_http_version(std::string& line, size_t s_offset)
{
	char major_version;
	char minor_version;

	if (line.size() - s_offset != 8)
		return false;
	if (line.compare(s_offset, 5, "HTTP/"))
		return false;
	s_offset += 5;
	major_version = line[s_offset];
	s_offset++;
	if (line[s_offset] != '.')
		return false;
	s_offset++;
	minor_version = line[s_offset];
	if (major_version != '1' || !std::isdigit(minor_version))
		return false;
	return true;
}

void RequestHeader::parse_request_line(std::string& line)
{
	size_t s_offset = 0;
	size_t e_offset;
	this->m_method = request_parse_method(line, s_offset);
	e_offset = s_offset;
	while (e_offset < line.size())
	{
		if (line[e_offset] == ' ')
			break;
		e_offset++;
	}
	if (e_offset == s_offset)
		throw WebservExceptions::BadRequest();
	this->m_target = line.substr(s_offset, e_offset - s_offset);
	size_t query_parameters_offset = this->m_target.find('?');
	if (query_parameters_offset != std::string::npos)
	{
		this->m_is_query_paramaters = true;
		this->m_query_parameters = this->m_target.substr(query_parameters_offset + 1);
		this->m_target.erase(query_parameters_offset);
	}
	e_offset++;
	s_offset = e_offset;
	if (!validate_http_version(line, s_offset))
		throw WebservExceptions::BadRequest();
}

RequestHeaderField split_request_header(std::string& line)
{
	RequestHeaderField field;
	size_t s_offset = 0;
	size_t e_offset = 0;

	while (e_offset < line.size())
	{
		if (line[e_offset] == ':')
			break;
		e_offset++;
	}
	if (!e_offset || e_offset == line.size())
		throw WebservExceptions::BadRequest();
	field.name = line.substr(s_offset, e_offset);
	s_offset = e_offset + 1;
	if (s_offset == line.size())
		return field;
	while (s_offset < line.size())
	{
		if (!is_ws_chr(line[s_offset]))
			break;
		s_offset++;
	}
	if (s_offset == line.size())
		return field;
	e_offset = line.size() - 1;
	while (e_offset > s_offset)
	{
		if (!is_ws_chr(line[e_offset]))
			break;
		e_offset--;
	}
	field.value = line.substr(s_offset, (e_offset - s_offset + 1));
	return field;
}

char c_tolower(char c)
{
	if (c >= 0x41 && c <= 0x5A)
		return c + 0x20;
	return c;
}

void RequestHeader::parse_header_line(std::string& line)
{
	RequestHeaderField field = split_request_header(line);
	if (!check_str_chrs(field.name, is_token_chr))
		throw WebservExceptions::BadRequest();
	if (!check_str_chrs(field.value, is_field_value_chr))
		throw WebservExceptions::BadRequest();
	std::transform(field.name.begin(), field.name.end(), field.name.begin(), c_tolower);
	if (this->m_fields.find(field.name) != this->m_fields.end())
	{
		if (field.name == "cookie")
			this->m_fields[field.name].append("; ");
		else
			this->m_fields[field.name].append(", ");
		this->m_fields[field.name].append(field.value);
	}
	else
		this->m_fields[field.name] = field.value;
}

void RequestHeader::parse_content_len()
{
	char *endptr;
	std::string& value = this->m_fields["content-length"];
	if (value.empty() || !std::isdigit(value[0]))
		throw WebservExceptions::BadRequest();
	errno = 0;
	this->m_content_len = strtoul(value.c_str(), &endptr, 10);
	if (errno == ERANGE || endptr != value.c_str() + value.size())
		throw WebservExceptions::BadRequest();
}

void RequestHeader::parse_transfer_encoding()
{
	if (this->m_fields.find("content-length") != this->m_fields.end())
		throw WebservExceptions::BadRequest();
	std::string temp_value = this->m_fields["transfer-encoding"];
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);
	if (temp_value != "chunked")
		throw WebservExceptions::NotImplemented();
	this->m_is_chunked = true;
}

void RequestHeader::parse_connection()
{
	std::string temp_value = this->m_fields["connection"];
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);
	if (temp_value.find("close") != std::string::npos)
		this->m_connection = CONNECTION_CLOSE;
}

void RequestHeader::parse(std::string& input)
{
	size_t line_start = 0;
	std::string line;
	while (true)
	{
		size_t clrf_pos = input.find("\r\n", line_start);
		if (clrf_pos == line_start)
			break;
		line = input.substr(line_start, clrf_pos - line_start);
		if (!line_start)
			parse_request_line(line);
		else
			parse_header_line(line);
		line_start = clrf_pos + 2;
	}
	if (this->m_fields.find("host") == this->m_fields.end())
		throw WebservExceptions::BadRequest();
	if (this->m_fields.find("content-length") != this->m_fields.end())
		parse_content_len();
	if (this->m_fields.find("transfer-encoding") != this->m_fields.end())
		parse_transfer_encoding();
	if (this->m_fields.find("connection") != this->m_fields.end())
		parse_connection();
}

void RequestHeader::debug()
{
	std::cout << "Method: " << this->m_method << std::endl;
	std::cout << "Target: " << this->m_target << std::endl;
	std::cout << "Is Chunked: " << (this->m_is_chunked ? "Yes" : "No") << std::endl;
	std::cout << "Connection: " << (this->m_connection ? "close" : "keep-alive") << std::endl;
	std::cout << "Content-Length: " << this->m_content_len << std::endl;
	if (this->m_is_query_paramaters)
		std::cout << "Query Parameters: " << this->m_query_parameters << std::endl;
	std::cout << "---------Fields---------" << std::endl;
	for (std::map<std::string, std::string>::iterator it = this->m_fields.begin();
		it != this->m_fields.end(); it++)
	{
		std::pair<std::string, std::string> entry = *it;
		std::cout << entry.first << ": " << entry.second << std::endl;
	}
}