/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 01:25:41 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/25 21:03:32 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef REQUESTBUFFER_HPP
#define REQUESTBUFFER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <string>

class RequestBuffer
{
	private:
		std::deque<std::string> m_chunks;
		size_t header_end_cursor;
	public:
		RequestBuffer();
		~RequestBuffer();
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