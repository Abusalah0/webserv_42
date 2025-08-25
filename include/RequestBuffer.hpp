/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 01:25:41 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/25 05:04:04 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTBUFFER_HPP
#define REQUESTBUFFER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <string>

class RequestBuffer
{
	private:
		std::deque<std::string> m_chunks;
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
};

#endif