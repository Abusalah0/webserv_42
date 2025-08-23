/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 02:12:16 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/23 04:18:01 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/RequestBuffer.hpp"

RequestBuffer::RequestBuffer(bool is_expandable):
	m_is_expandable(is_expandable),
	m_push_current_chunk(0),
	m_pull_current_chunk(0),
	m_chunks()
{}

RequestBuffer::~RequestBuffer()
{}

void RequestBuffer::push(char *buf, size_t len)
{
	
}

std::string RequestBuffer::pop()
{
	std::string chunk = this->m_chunks[0];
	this->m_chunks.pop_front();
	return chunk;
}

void RequestBuffer::remove_chunk()
{
	this->m_chunks.pop_front();
}