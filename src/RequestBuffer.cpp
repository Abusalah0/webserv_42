/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 02:12:16 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/25 05:03:59 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestBuffer.hpp"

RequestBuffer::RequestBuffer():
	m_chunks()
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
	if (this->m_chunks[0].find("\r\n\r\n") != std::string::npos)
		return true;
	return false;
}