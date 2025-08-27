#include "../include/RequestHeader.hpp"

RequestHeader::RequestHeader()
{}

RequestHeader::~RequestHeader()
{}

void RequestHeader::parse_request_line(std::string& line)
{
	
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