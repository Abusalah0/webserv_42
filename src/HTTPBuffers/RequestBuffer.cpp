/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 02:12:16 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 01:20:37 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <RequestBuffer.hpp>

RequestBuffer::RequestBuffer():
	HTTPBuffer(),
	m_header_end_cursor(0)
{}

RequestBuffer::~RequestBuffer()
{}

bool RequestBuffer::is_header_finished()
{
	size_t pos = this->m_chunks[0].find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->m_header_end_cursor = pos + 4;
		return true;
	}
	return false;
}

void RequestBuffer::isolate_header()
{
	if (this->m_header_end_cursor >= this->m_chunks[0].size())
		return;
	std::string part = this->m_chunks[0].substr(this->m_header_end_cursor, std::string::npos);
	this->m_chunks[0].erase(this->m_header_end_cursor);
	if (this->m_chunks.size() < 2)
		add_chunk();
	this->m_chunks[1].insert(0, part);
}