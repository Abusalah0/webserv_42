/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:11:11 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/08 01:05:59 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef REQUESTHEADER_HPP
#define REQUESTHEADER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <map>

typedef struct SHTTPHeaderField
{
	std::string name;
	std::string value;
} HTTPHeaderField;

class HTTPHeader
{
	private:
		bool m_is_query_paramaters;
		bool m_is_chunked;
		bool m_ignore_content_len_field;
		ConnectionTypes m_connection;
		std::string m_method;
		size_t m_content_len;
		std::string m_target;
		std::string m_aug_target;
		std::string m_query_parameters;
		std::string m_virtual_host;
		std::string m_response_msg;
    	std::map<std::string, HTTPHeaderField> m_fields;
		std::deque<HTTPHeaderField> m_response_fields;
		void parse_request_line(std::string& line);
		void parse_request_header_line(std::string& line);
		void parse_response_header_line(std::string& line);
		void parse_content_len();
		void parse_transfer_encoding();
		void parse_connection();
	public:
		/**
 		* RequestHeader Constructor
 		* @return RequestHeader
		*/
		HTTPHeader();
		/**
 		* RequestHeader Destructor
		*/
		~HTTPHeader();
		/**
 		* Parses Request Header
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
		void add_field(HTTPHeaderField& field);
		void clear();
		void debug();
		// void merge_cgi_fields(HTTPHeader& cgi_header);
		void set_chunked();
		std::string& get_aug_target();
		void set_aug_target(const std::string& str);
};

#endif