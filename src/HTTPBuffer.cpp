/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:19:47 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/19 23:31:29 by abdsalah         ###   ########.fr       */
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
	size_t barriers_size = this->m_barriers.size(); // Get the current number of barriers
	
	// Only create a barrier if there are no barriers or the last barrier is not at the end of the buffer
	// This prevents duplicate barriers at the same position
	if (barriers_size && this->m_barriers[barriers_size - 1] != this->m_data.size())
		this->m_barriers.push_back(this->m_data.size());
}

void HTTPBuffer::erase(size_t n)
{
	this->m_data.erase(0, n);
}

std::string HTTPBuffer::pull(size_t n)
{
	// Respect barriers: pull up to the first barrier if it exists and n exceeds it
	if (this->m_barriers.size() && n > this->m_barriers[0])
	{
		n = this->m_barriers[0]; // Limit to barrier position
		this->m_barriers.pop_front(); // Remove the barrier we just reached
	}

	std::string data = this->m_data.substr(0, n); // Extract the data to be pulled
	this->m_data.erase(0, n); // Remove it from the buffer
	
	// Update all remaining barriers by subtracting the pulled amount
	size_t i = 0;
	while (i < this->m_barriers.size())
	{
		this->m_barriers[i] -= n; // Adjust barrier position

		if (!this->m_barriers[i]) // If the barrier reached position 0, remove it
			this->m_barriers.erase(this->m_barriers.begin() + i);
		else
			++i; // Only increment if we didn't erase an element
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

	// Limit search to CHUNK_SIZE for performance (headers shouldn't be huge)
	if (this->m_data.size() <= CHUNK_SIZE)
		limit_it = this->m_data.end(); // Search entire buffer if smaller than CHUNK_SIZE
	else
		limit_it = this->m_data.begin() + CHUNK_SIZE; // Limit search to CHUNK_SIZE bytes
	
	// Search for the double CRLF sequence ("\r\n\r\n") that marks end of HTTP header
	std::string::iterator it = std::search(this->m_data.begin(), limit_it, dcrlf.begin(), dcrlf.begin() + 4);
	
	if (it != limit_it) // If the sequence was found within our search range
	{
		// Set the header end cursor to the position after the found sequence
		this->m_header_end_cursor = std::distance(this->m_data.begin(), it) + 4;
		return (true);
	}

	return (false); // Header not complete yet
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
