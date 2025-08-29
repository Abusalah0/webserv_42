/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:16:02 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/30 01:22:29 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef RESPONSEBUFFER_HPP
#define RESPONSEBUFFER_HPP

#include "HTTPBuffer.hpp"

class ResponseBuffer: public HTTPBuffer
{
	private:
		size_t m_is_finished;
	public:
		ResponseBuffer();
		~ResponseBuffer();
		void set_finished();
		bool is_finished();
};

#endif