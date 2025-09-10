/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:09:42 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/10 20:29:09 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPBUFFER_HPP
#define HTTPBUFFER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <string>

static const std::string dcrlf = "\r\n\r\n";

class HTTPBuffer
{
	private:
		std::string m_data;
		std::deque<size_t> m_barriers;
		size_t m_header_end_cursor;
		size_t m_encoded_cursor;
	public:
		HTTPBuffer();
		virtual ~HTTPBuffer();
		void push(const char *buf, size_t len);
		void create_barrier();
		void erase(size_t n);
		std::string pull(size_t n);
		size_t size();
		bool is_header_finished();
		bool is_crlf_found();
		void header_lf_to_crlf();
		std::string pull_header();
		std::string pull_encoded();
};

#endif