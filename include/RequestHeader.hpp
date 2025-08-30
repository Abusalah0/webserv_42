/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHeader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:11:11 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 19:02:43 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef REQUESTHEADER_HPP
#define REQUESTHEADER_HPP

#include "CommonUtils.hpp"
#include <map>

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
		std::string m_method;
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
		std::map<std::string, std::string> get_fields();
		void clear();
		void debug();
};

#endif