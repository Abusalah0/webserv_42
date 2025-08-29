#include "CommonUtils.hpp"
#include <map>
#include <deque>

typedef enum ERequestMethods
{
	GET_METHOD,
	POST_METHOD,
	DELETE_METHOD
} RequestMethods;

typedef struct SRequestHeaderField
{
	std::string name;
	std::string value;
} RequestHeaderField;

class RequestHeader
{
	private:
		bool m_is_query_paramaters;
		bool m_is_chunked;
		ConnectionTypes m_connection;
		RequestMethods m_method;
		size_t m_content_len;
		std::string m_target;
		std::string m_query_parameters;
		std::string m_virtual_host;
    	std::map<std::string, std::string> m_fields;
		void parse_request_line(std::string& line);
		void parse_header_line(std::string& line);
		void parse_content_len();
		void parse_transfer_encoding();
		void parse_connection();
	public:
		RequestHeader();
		~RequestHeader();
		void parse(std::string& input);
		bool IsQueryParameters();
		bool isChunked();
		ConnectionTypes getConnectionType();
		RequestMethods getRequestMethod();
		size_t getContentLength();
		std::string& getTarget();
		std::string& getQueryParameters();
		std::string& getVirtualHost();
		std::map<std::string, std::string> getFields();
		void debug();
};