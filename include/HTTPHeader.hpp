/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:11:11 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/19 17:59:25 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHEADER_HPP
# define REQUESTHEADER_HPP

# include "CommonUtils.hpp"
# include <deque>
# include <map>
# include <set>

/**
 * @brief struct representing a single HTTP header field.
 * It contains the name and value of the field.
 * @note The name is case-insensitive.	
 * @note The value may contain leading or trailing whitespace, which should be trimmed.
 */
typedef struct SHTTPHeaderField
{
	std::string name;
	std::string value;
} HTTPHeaderField;

class HTTPHeader
{
	private:
		bool m_is_query_paramaters;// true if the request target contains query parameters
		bool m_is_chunked;// true if the request body is chunked
		bool m_ignore_content_len_field;// true if the content length field should be ignored
		ConnectionTypes m_connection;// the connection type (keep-alive or close)
		std::string m_method;// the request method (GET, POST, DELETE, etc.)
		size_t m_content_len;// the content length of the request body
		std::string m_target;// the request target (URI)
		std::string m_aug_target;// the augmented target (may be modified during processing)
		std::string m_query_parameters;// the query parameters from the request target
		std::string m_virtual_host;// the virtual host from the Host header
		std::string m_response_msg;// the response message (e.g., "OK", "Not Found", etc.)
    	std::map<std::string, HTTPHeaderField> m_fields;// map of header fields
		std::deque<HTTPHeaderField> m_response_fields;// deque of response header fields
		std::string m_allowed_methods;// allowed methods for 405 response
		std::string m_last_modified;// last modified time for response
		
		/**
		 * @brief Parses the request line of an HTTP request.
		 * and sets member variables such as 
		 * m_method, m_target, and m_http_version,
		 * m_is_query_paramaters, m_query_parameters.
		 * @param line The request line to parse.
		 * @throws WebservExceptions::BadRequest on invalid input.
		 * @throws std::bad_alloc on allocation failure.
		 * @return void
		 */
		void parse_request_line(std::string& line);

		/**
		 * @brief Parses a single header line from an HTTP request.
		 * and adds it to the m_fields map.
		 * @param line The header line to parse.
		 * @throws WebservExceptions::BadRequest on invalid input.
		 * @throws std::bad_alloc on allocation failure.
		 * @return void
		 */
		void parse_request_header_line(std::string& line);
		void parse_response_header_line(std::string& line);
		void parse_content_len();
		void parse_transfer_encoding();
		void parse_connection();
	public:
		// constructors and destructor
		HTTPHeader();
		~HTTPHeader();
		
		/**
 		* @brief Parses Request Header
		* @param input Request Header
		*/
		void parse_request(std::string& input);
		void parse_response(std::string& input);
		
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
		std::string& get_request_method();
		
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
		std::map<std::string, HTTPHeaderField>& get_fields();
		/**
		 * @brief Sets the content length for the header.
		 * @param len The content length to set.
		 */
		void set_content_length(size_t len);
		/**
		 * @brief Gets map of response fields.
		 * @return Deque of response fields.
		 */
		std::deque<HTTPHeaderField>& get_response_fields();
		/**
		 * @brief Generates the response header as a string.
		 * @return The generated response header string.
		 */
		std::string generate_response_header();
		/**
		 * @brief Generates the response fields based on the client status and message.
		 * @param client_status The status code of the client.
		 * @param msg The message associated with the status.
		 * @param is_chunked Indicates if the response is chunked.
		 * @param media_type provides the MIME.
		 */
		void ignore_content_len_field();
		void generate_response_fields(int client_status,
			const std::string& msg,
			bool is_chunked,
			const char* media_type = "");
		/**
		 * @brief Parses the Content-Length header from the response.
		 */
		void parse_response_content_len();
		/**
		 * @brief Clears the header fields and resets member variables.
		 */
		// void add_field(HTTPHeaderField& field);
		void add_field(const std::string& name, const std::string& value);
		void clear();
		void debug();
		// void merge_cgi_fields(HTTPHeader& cgi_header);
		void set_chunked();
		std::string& get_aug_target();
		void set_aug_target(const std::string& str);
		void set_last_modified(time_t raw_time);
		void set_allowed_methods(const std::set<std::string>& allowed_methods);
};

#endif
