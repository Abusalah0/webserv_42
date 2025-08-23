/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 01:25:41 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/23 04:10:49 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef REQUESTBUFFER_HPP
#define REQUESTBUFFER_HPP

#define CHUNK_SIZE KILOBYTE * 8

#include "CommonUtils.hpp"
#include <deque>
#include <string>

class RequestBuffer
{
	private:
		bool m_is_expandable;
		size_t m_push_current_chunk;
		size_t m_pull_current_chunk;
		std::deque<std::string> m_chunks;
	public:
		RequestBuffer(bool is_expandable);
		~RequestBuffer();
		void push(char *buf, size_t len);
		std::string pop();
		void remove_chunk();
		void clear();
};

#endif