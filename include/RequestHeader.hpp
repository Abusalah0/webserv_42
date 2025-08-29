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
		/**
 		* RequestHeader Constructor
 		* @return RequestHeader
		*/
		RequestHeader();
		/**
 		* RequestHeader Destructor
		*/
		~RequestHeader();
		/**
 		* Parses Request Header
		* @param input Request Header
		*/
		void parse(std::string& input);
		/**
 		* Getter for if query parameters set
		* @throws WebservExceptions::BadRequest on invalid input
		* @throws WebservExceptions::NotImplemented on unsupported input
		* @throws std::bad_alloc on allocation failure
		* @return If query parameters set
		*/
		bool is_query_parameters();
		/**
 		* Getter for if request body is chunked
		* @return If request body is chunked
		*/
		bool is_chunked();
		/**
 		* Getter for connection type
		* @return connection type
		*/
		ConnectionTypes get_connection_type();
		/**
 		* Getter for request method
		* @return request method
		*/
		RequestMethods get_request_method();
		/**
 		* Getter for content length
		* @return content length
		*/
		size_t get_content_length();
		/**
 		* Getter for request target
		* @return request target
		*/
		std::string& get_target();
		/**
 		* Getter for query parameters
		* @return query parameters
		*/
		std::string& get_query_parameters();
		/**
 		* Getter for virtual host
		* @return virtual host
		*/
		std::string& get_virtual_host();
		/**
 		* Getter for header fields
		* @return header fields
		*/
		std::map<std::string, std::string> get_fields();
		void debug();
};