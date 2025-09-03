/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:19:47 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/03 16:35:27 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HTTPBuffer.hpp"
#include <iostream>
#include <algorithm>

HTTPBuffer::HTTPBuffer():
	m_data(),
	m_barriers(),
	m_header_end_cursor(0),
	m_encoded_cursor(0)
{
	m_data.reserve(CHUNK_SIZE);
}

HTTPBuffer::~HTTPBuffer()
{}

void HTTPBuffer::push(const char *buf, size_t len)
{
	this->m_data.append(buf, len);
}

void HTTPBuffer::create_barrier()
{
	size_t barriers_size = this->m_barriers.size();
	if (barriers_size && this->m_barriers[barriers_size - 1] != this->m_data.size())
		this->m_barriers.push_back(this->m_data.size());
}

void HTTPBuffer::erase(size_t n)
{
	this->m_data.erase(0, n);
}

std::string HTTPBuffer::pull(size_t n)
{
	if (this->m_barriers.size() && n > this->m_barriers[0])
	{
		n = this->m_barriers[0];
		this->m_barriers.pop_front();
	}
	std::string data = this->m_data.substr(0, n);
	this->m_data.erase(0, n);
	size_t i = 0;
	while (i < this->m_barriers.size())
	{
		this->m_barriers[i] -= n;
		if (!this->m_barriers[i])
			this->m_barriers.erase(this->m_barriers.begin() + i);
		else
			++i;
	}
	return data;
}

std::string HTTPBuffer::pull_header()
{
	std::string header = this->m_data.substr(0, this->m_header_end_cursor);
	this->m_data.erase(0, this->m_header_end_cursor);
	return header;
}

std::string HTTPBuffer::pull_encoded()
{
	std::string encoded = this->m_data.substr(0, this->m_encoded_cursor);
	this->m_data.erase(0, this->m_encoded_cursor);
	return encoded;
}

size_t HTTPBuffer::size()
{
	return this->m_data.size();
}

bool HTTPBuffer::is_header_finished()
{
	std::string::iterator limit_it;
	if (this->m_data.size() <= CHUNK_SIZE)
		limit_it = this->m_data.end();
	else
		limit_it = this->m_data.begin() + CHUNK_SIZE;
	
	std::string::iterator it = std::search(this->m_data.begin(), limit_it, dclrf.begin(), dclrf.begin() + 4);
	if (it != limit_it)
	{
		this->m_header_end_cursor = std::distance(this->m_data.begin(), it) + 4;
		return true;
	}
	return false;
}

bool HTTPBuffer::is_clrf_found()
{
	size_t pos = this->m_data.find("\r\n");
	if (pos != std::string::npos)
	{
		this->m_encoded_cursor = pos;
		return true;
	}
	return false;
}