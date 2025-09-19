/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:19:47 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/19 15:54:27 by abdsalah         ###   ########.fr       */
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
	size_t barriers_size = this->m_barriers.size();// get the current number of barriers
	// only create a barrier if there are no barriers or the last barrier is not at the end of the buffer
	if (barriers_size && this->m_barriers[barriers_size - 1] != this->m_data.size())
		this->m_barriers.push_back(this->m_data.size());
}

void HTTPBuffer::erase(size_t n)
{
	this->m_data.erase(0, n);
}

std::string HTTPBuffer::pull(size_t n)
{
	// pull up to the first barrier if exists and n is larger than it
	if (this->m_barriers.size() && n > this->m_barriers[0])
	{
		n = this->m_barriers[0];
		this->m_barriers.pop_front();
	}

	std::string data = this->m_data.substr(0, n);// get the data to be pulled
	this->m_data.erase(0, n);// erase it from the buffer
	size_t i = 0;

	while (i < this->m_barriers.size())
	{
		this->m_barriers[i] -= n;

		if (!this->m_barriers[i])// if the barrier reached 0, remove it
			this->m_barriers.erase(this->m_barriers.begin() + i);
		else
			++i;
	}

	return (data);
}

std::string HTTPBuffer::pull_header()
{
	std::string header = this->m_data.substr(0, this->m_header_end_cursor);// get the header
	this->m_data.erase(0, this->m_header_end_cursor);// erase it from the buffer

	return (header);
}

std::string HTTPBuffer::pull_encoded()
{
	std::string encoded = this->m_data.substr(0, this->m_encoded_cursor);// get the encoded part
	this->m_data.erase(0, this->m_encoded_cursor);// erase it from the buffer

	return (encoded);
}

size_t HTTPBuffer::size()
{
	return (this->m_data.size());
}

bool HTTPBuffer::is_header_finished()
{
	std::string::iterator limit_it;

	if (this->m_data.size() <= CHUNK_SIZE)// if the buffer size is less than CHUNK_SIZE
		limit_it = this->m_data.end();// set the limit to the end of the buffer
	else
		limit_it = this->m_data.begin() + CHUNK_SIZE;// set the limit to CHUNK_SIZE
	// search for the double CRLF sequence
	std::string::iterator it = std::search(this->m_data.begin(), limit_it, dcrlf.begin(), dcrlf.begin() + 4);
	if (it != limit_it)// if found
	{
		// set the header end cursor to the position after the found sequence
		this->m_header_end_cursor = std::distance(this->m_data.begin(), it) + 4;
		return (true);
	}

	return (false);
}

void HTTPBuffer::header_lf_to_crlf()
{
	size_t pos = this->m_data.find('\n');// find the first LF character
	// insert a CR before each LF that is not preceded by a CR
	// stop if we reach a double CRLF sequence
	while (pos != std::string::npos)
	{
		if (pos == 0 || this->m_data[pos - 1] != '\r')
		{
			this->m_data.insert(this->m_data.begin() + pos, '\r');
			pos++;
		}
		if (pos > 2)
		{
			if (!this->m_data.compare(pos - 3, 4, "\r\n\r\n", 4))// if we found a double CRLF sequence
				return ;
		}
		pos = this->m_data.find('\n', pos + 1);// find the next LF character
	}
}

bool HTTPBuffer::is_crlf_found()
{
	size_t pos = this->m_data.find("\r\n");
	// set the cursor to the position after the found CRLF sequence
	if (pos != std::string::npos)
	{
		this->m_encoded_cursor = pos;
		return (true);
	}
	return (false);
}
