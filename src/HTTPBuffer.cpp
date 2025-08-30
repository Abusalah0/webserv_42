/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:19:47 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 06:06:32 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <HTTPBuffer.hpp>

HTTPBuffer::HTTPBuffer():
	m_chunks()
{}

HTTPBuffer::~HTTPBuffer()
{}

void HTTPBuffer::add_chunk()
{
	std::string chunk;
	chunk.reserve(CHUNK_SIZE);
	this->m_chunks.push_back(chunk);
}

void HTTPBuffer::push(const char *buf, size_t len)
{
	if (!this->m_chunks.size())
		add_chunk();
	size_t index = this->m_chunks.size() - 1;
	size_t offset = 0;
	if (len + this->m_chunks[index].size() > CHUNK_SIZE)
	{
		add_chunk();
		offset = CHUNK_SIZE - this->m_chunks[index].size();
		this->m_chunks[index].append(buf, offset);
		len -= offset;
		index++;
	}
	this->m_chunks[index].append(buf + offset, len);
}

std::string HTTPBuffer::pop()
{
	std::string chunk = this->m_chunks[0];
	this->m_chunks.pop_front();
	return chunk;
}

size_t HTTPBuffer::size()
{
	return this->m_chunks.size();
}

void HTTPBuffer::remove_chunk()
{
	this->m_chunks.pop_front();
}

bool HTTPBuffer::is_header_finished()
{
	size_t pos = this->m_chunks[0].find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->m_header_end_cursor = pos + 4;
		return true;
	}
	return false;
}

void HTTPBuffer::isolate_header()
{
	if (this->m_header_end_cursor >= this->m_chunks[0].size())
		return;
	std::string part = this->m_chunks[0].substr(this->m_header_end_cursor, std::string::npos);
	this->m_chunks[0].erase(this->m_header_end_cursor);
	if (this->m_chunks.size() < 2)
		add_chunk();
	this->m_chunks[1].insert(0, part);
}