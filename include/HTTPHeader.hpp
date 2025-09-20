/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:11:11 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/20 02:51:34 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHEADER_HPP
# define REQUESTHEADER_HPP

# include "CommonUtils.hpp"
# include <deque>
# include <map>
# include <set>

/**
 * @brief Structure representing a single HTTP header field.
 * Contains the name and value of an HTTP header field as defined in RFC 7230.
 * @note Header names are case-insensitive according to HTTP specification.
 * @note Header values may contain leading or trailing whitespace that should be trimmed.
 */
typedef struct SHTTPHeaderField
{
	std::string name;  ///< Header field name (e.g., "Content-Type", "User-Agent")
	std::string value; ///< Header field value (e.g., "text/html", "Mozilla/5.0")
} HTTPHeaderField;

/**
 * @brief HTTP header parser and generator for request/response processing.
 * 
 * This class handles both HTTP request and response headers, providing:
 * - Request line parsing (method, target, HTTP version)
 * - Header field parsing and validation
 * - Query parameter extraction
 * - Transfer encoding detection (chunked vs content-length)
 * - Connection type management (keep-alive vs close)
 * - Response header generation with proper formatting
 * - Virtual host resolution for server selection
 */
class HTTPHeader
{
	private:
		// Request parsing state
		bool m_is_query_paramaters; ///< True if request target contains query parameters
		bool m_is_chunked; ///< True if request uses chunked transfer encoding
		bool m_ignore_content_len_field; ///< True if Content-Length should be ignored (chunked takes precedence)
		ConnectionTypes m_connection; ///< Connection type (keep-alive or close)
		
		// Request data
		std::string m_method; ///< HTTP method (GET, POST, DELETE, etc.)
		size_t m_content_len; ///< Content length of request/response body
		std::string m_target; ///< Request target URI (normalized and decoded)
		std::string m_aug_target; ///< Augmented target (modified during location processing)
		std::string m_query_parameters; ///< Query string from request target
		std::string m_virtual_host; ///< Virtual host from Host header
		
		// Response generation
		std::string m_response_msg; ///< HTTP status message (e.g., "OK", "Not Found")
    	std::map<std::string, HTTPHeaderField> m_fields; ///< Map of parsed header fields (lowercase keys)
		std::deque<HTTPHeaderField> m_response_fields; ///< Response headers to be sent
		std::string m_allowed_methods; ///< Allowed methods string for 405 responses
		std::string m_last_modified; ///< Last-Modified header value for responses
		
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
		 * Extracts header name and value, performs validation, and adds to the fields map.
		 * @param line The header line to parse (format: "Name: Value")
		 * @throws WebservExceptions::BadRequest on invalid header format
		 * @throws std::bad_alloc on memory allocation failure
		 */
		void parse_request_header_line(std::string& line);
		
		/**
		 * @brief Parses a single header line from an HTTP response.
		 * Used for processing CGI script output headers.
		 * @param line The response header line to parse
		 */
		void parse_response_header_line(std::string& line);
		
		/**
		 * @brief Parses and validates the Content-Length header field.
		 * Extracts numeric content length and sets internal state.
		 */
		void parse_content_len();
		
		/**
		 * @brief Parses the Transfer-Encoding header to detect chunked encoding.
		 * Sets chunked transfer flag if "chunked" encoding is specified.
		 */
		void parse_transfer_encoding();
		
		/**
		 * @brief Parses the Connection header to determine connection persistence.
		 * Sets connection type to keep-alive or close based on header value.
		 */
		void parse_connection();
	public:
		// Constructors and Destructor
		HTTPHeader();
		~HTTPHeader();
		
		// Request/Response Parsing
		/**
		 * @brief Parses a complete HTTP request header.
		 * Processes request line and all header fields, validating format and extracting data.
		 * @param input Complete HTTP request header string (including request line)
		 * @throws WebservExceptions::BadRequest on malformed request
		 * @throws WebservExceptions::NotImplemented on unsupported HTTP version
		 * @throws std::bad_alloc on memory allocation failure
		 */
		void parse_request(std::string& input);
		
		/**
		 * @brief Parses HTTP response headers from CGI output.
		 * Processes response headers generated by CGI scripts.
		 * @param input HTTP response header string from CGI script
		 */
		void parse_response(std::string& input);
		
		// Request Property Getters
		/**
		 * @brief Checks if the request target contains query parameters.
		 * @return true if query parameters are present in the request target, false otherwise
		 */
		bool is_query_parameters();
		
		/**
		 * @brief Checks if the request uses chunked transfer encoding.
		 * @return true if Transfer-Encoding: chunked is specified, false otherwise
		 */
		bool is_chunked();
		
		/**
		 * @brief Gets the connection type for the request.
		 * @return CONNECTION_KEEP_ALIVE or CONNECTION_CLOSE based on Connection header
		 */
		ConnectionTypes get_connection_type();
		
		/**
		 * @brief Gets the HTTP method of the request.
		 * @return Reference to HTTP method string (e.g., "GET", "POST", "DELETE")
		 */
		std::string& get_request_method();
		
		/**
		 * @brief Gets the content length of the request body.
		 * @return Content length in bytes, or 0 if not specified
		 */
		size_t get_content_length();
		
		/**
		 * @brief Gets the request target URI (normalized and decoded).
		 * @return Reference to the request target string (e.g., "/index.html")
		 */
		std::string& get_target();
		
		/**
		 * @brief Gets the query parameters from the request target.
		 * @return Reference to query string (without the '?' prefix)
		 */
		std::string& get_query_parameters();
		
		/**
		 * @brief Gets the virtual host from the Host header.
		 * @return Reference to virtual host string for server selection
		 */
		std::string& get_virtual_host();
		
		/**
		 * @brief Gets the map of all parsed header fields.
		 * @return Reference to map with lowercase header names as keys
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
