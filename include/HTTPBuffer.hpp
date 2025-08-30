/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:09:42 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 06:07:31 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPBUFFER_HPP
#define HTTPBUFFER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <string>

class HTTPBuffer
{
	protected:
		std::deque<std::string> m_chunks;
		size_t m_header_end_cursor;
	public:
		HTTPBuffer();
		virtual ~HTTPBuffer();
		void push(const char *buf, size_t len);
		std::string pop();
		size_t size();
		void remove_chunk();
		void clear();
		void add_chunk();
		bool is_header_finished();
		void isolate_header();
};

#endif