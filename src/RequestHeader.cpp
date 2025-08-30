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

std::string request_parse_method(std::string& line, size_t& offset)
{
	size_t i = 0;
	if (line.find(' ') == std::string::npos)
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	while (line[i] != ' ')
		++i;
	if (!i)
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	std::string method = line.substr(0, i);
	offset = i + 1;
	return method;
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
	if (s_offset + 1 < line.size())
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	if (!check_str_chrs(field.value, is_field_value_chr))
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	std::transform(field.name.begin(), field.name.end(), field.name.begin(), c_tolower);
	if (this->m_fields.find(field.name) != this->m_fields.end())
	{
		if (field.name == "content-length")
			throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	errno = 0;
	this->m_content_len = strtoul(value.c_str(), &endptr, 10);
	if (errno == ERANGE || endptr != value.c_str() + value.size())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
}

void RequestHeader::parse_transfer_encoding()
{
	if (this->m_fields.find("content-length") != this->m_fields.end())
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	std::string temp_value = this->m_fields["transfer-encoding"];
	std::transform(temp_value.begin(), temp_value.end(), temp_value.begin(), c_tolower);
	if (temp_value != "chunked")
		throw WebservExceptions::HTTPException(HTTP_NOT_IMPLEMENTED);
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
		throw WebservExceptions::HTTPException(HTTP_BAD_REQUEST);
	if (this->m_fields.find("content-length") != this->m_fields.end())
		parse_content_len();
	if (this->m_fields.find("transfer-encoding") != this->m_fields.end())
		parse_transfer_encoding();
	if (this->m_fields.find("connection") != this->m_fields.end())
		parse_connection();
}

bool RequestHeader::is_query_parameters()
{
	return this->m_is_query_paramaters;
}

bool RequestHeader::is_chunked()
{
	return this->m_is_chunked;
}

ConnectionTypes RequestHeader::get_connection_type()
{
	return this->m_connection;
}

std::string& RequestHeader::get_request_method()
{
	return this->m_method;
}

size_t RequestHeader::get_content_length()
{
	return this->m_content_len;
}

std::string& RequestHeader::get_target()
{
	return this->m_target;
}

std::string& RequestHeader::get_query_parameters()
{
	return this->m_query_parameters;
}

std::string& RequestHeader::get_virtual_host()
{
	return this->m_virtual_host;
}

std::map<std::string, std::string> RequestHeader::get_fields()
{
	return this->m_fields;
}

void RequestHeader::clear()
{
	this->m_is_query_paramaters = false;
	this->m_is_chunked = false;
	this->m_connection = CONNECTION_KEEP_ALIVE;
	this->m_content_len = 0;
	this->m_query_parameters.clear();
	this->m_fields.clear();
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