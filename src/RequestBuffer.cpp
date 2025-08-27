/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 02:12:16 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/27 17:42:33 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/RequestBuffer.hpp"

RequestBuffer::RequestBuffer():
	m_chunks(),
	header_end_cursor(0)
{}

RequestBuffer::~RequestBuffer()
{}

void RequestBuffer::add_chunk()
{
	std::string chunk;
	chunk.reserve(CHUNK_SIZE);
	this->m_chunks.push_back(chunk);
}

void RequestBuffer::push(const char *buf, size_t len)
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

std::string RequestBuffer::pop()
{
	std::string chunk = this->m_chunks[0];
	this->m_chunks.pop_front();
	return chunk;
}

size_t RequestBuffer::size()
{
	return this->m_chunks.size();
}

void RequestBuffer::remove_chunk()
{
	this->m_chunks.pop_front();
}

bool RequestBuffer::is_header_finished()
{
	size_t pos = this->m_chunks[0].find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->header_end_cursor = pos + 4;
		return true;
	}
	return false;
}

void RequestBuffer::isolate_header()
{
	if (this->header_end_cursor >= this->m_chunks[0].size())
		return;
	std::string part = this->m_chunks[0].substr(this->header_end_cursor, std::string::npos);
	this->m_chunks[0].erase(this->header_end_cursor);
	if (this->m_chunks.size() < 2)
		add_chunk();
	this->m_chunks[1].insert(0, part);
}