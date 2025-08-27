#include "CommonUtils.hpp"
#include <map>
#include <deque>

class RequestHeader
{
	private:
		int m_method;
		std::string m_route;
		std::string m_query_parameters;
		std::string m_virtual_host;
    	std::string m_cookies;
    	std::map<std::string, std::string> m_fields;
		void parse_request_line(std::string& line);
		void parse_header_line(std::string& line);
	public:
		RequestHeader();
		~RequestHeader();
		void parse(std::string& input);
};