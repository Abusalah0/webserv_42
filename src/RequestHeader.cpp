#include "../include/RequestHeader.hpp"
#include <vector>

RequestHeader::RequestHeader()
{}

RequestHeader::~RequestHeader()
{}

bool check_token_chr(char c)
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

// std::vector<std::string> split_rl(std::string& line)
// {
// 	size_t offset = 0;
// 	std::vector<std::string> tokens;
// 	while (offset < line.size())
// 	{
// 		if (offset)
// 		offset++;
// 	}
// }

RequestMethods request_parse_method(std::string& line, size_t& offset)
{
	if (line.compare(0, 4, "GET "))
	{
		offset = 4;
		return GET_METHOD;
	}
	if (line.compare(0, 5, "POST "))
	{
		offset = 5;
		return POST_METHOD;
	}
	if (line.compare(0, 7, "DELETE "))
	{
		offset = 7;
		return DELETE_METHOD;
	}
	throw WebservExceptions::InvalidValue();
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
	this->m_target = line.substr(s_offset, e_offset - s_offset);
	e_offset++;
	if ()
}

void RequestHeader::parse_header_line(std::string& line)
{
	
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
}