/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 02:12:16 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 01:22:42 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <ResponseBuffer.hpp>

ResponseBuffer::ResponseBuffer():
	HTTPBuffer(),
	m_is_finished(0)
{}

ResponseBuffer::~ResponseBuffer()
{}

void ResponseBuffer::set_finished()
{
	this->m_is_finished = true;
}

bool ResponseBuffer::is_finished()
{
	if (this->m_is_finished && !this->m_chunks.size())
		return true;
	return false;
}