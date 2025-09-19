/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 01:09:42 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/19 16:19:52 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPBUFFER_HPP
#define HTTPBUFFER_HPP

#include "CommonUtils.hpp"
#include <deque>
#include <string>

static const std::string dcrlf = "\r\n\r\n";

class HTTPBuffer
{
	private:
		std::string m_data;// the actual data buffer
		std::deque<size_t> m_barriers; // barriers to limit pulling data, each barrier is an index in m_data
		size_t m_header_end_cursor;// the position of the end of the header in the buffer, 0 if not found
		size_t m_encoded_cursor;// the position of the end of the encoded line in the buffer, 0 if not found

	public:
		// Constructors and Destructors
		HTTPBuffer();
		virtual ~HTTPBuffer();

		/**
		 * @brief Appends data to the buffer.
		 * @param buf Pointer to the data to be appended.
		 * @param len Length of the data to be appended.
		 * @return void
		 */
		void push(const char *buf, size_t len);

		/**
		 * @brief Creates a barrier at the current end of the buffer.
		 * Barriers are used to limit how much data can be pulled from the buffer.
		 * If the last barrier is already at the end of the buffer, no new barrier is created.
		 * @return void
		 */
		void create_barrier();

		/**
		 * @brief Erases the first n bytes from the buffer.
		 * @param n Number of bytes to erase from the beginning of the buffer.
		 * @return void
		 */
		void erase(size_t n);

		/**
		 * @brief Pulls up to n bytes from the buffer, respecting barriers.
		 * If a barrier exists and n exceeds the first barrier, only data up to the barrier
		 * is pulled and the barrier is removed.
		 * @param n Maximum number of bytes to pull from the buffer.
		 * @return A string containing the pulled data.
		 */
		std::string pull(size_t n);

		/**
		 * @brief Gets the current size of the buffer.
		 * @return The size of the buffer in bytes.
		 */
		size_t size();
		
		/**
		 * @brief Checks if the HTTP header in the buffer is complete.
		 * Searches for the double CRLF sequence that indicates the end of the header.
		 * Limits the search to the first CHUNK_SIZE bytes for efficiency.
		 * If found, sets the header end cursor to the position after the sequence.
		 * @return true if the header is complete, false otherwise.
		 */
		bool is_header_finished();
		
		/**
		 * @brief  Checks if a CRLF sequence is found in the buffer. and sets the encoded cursor to the position after it.
		 * @return true if a CRLF sequence is found, false otherwise.
		 */
		bool is_crlf_found();

		/**
		 * @brief Converts lone LF characters in the header to CRLF sequences.
		 * This is useful for normalizing line endings in HTTP headers.
		 * @return void
		 */
		void header_lf_to_crlf();

		/**
		 * @brief Pulls the complete HTTP header from the buffer using the header and end cursor . and removes it from the buffer.
		 * If the header is not complete, returns an empty string.
		 * @return A string containing the complete HTTP header, or an empty string if the header is incomplete.
		 */
		std::string pull_header();
		
		/**
		 * @brief Pulls data from the buffer up to the next CRLF sequence and removes it from the buffer.
		 * If no CRLF sequence is found, returns an empty string.
		 * @return A string containing the data up to the next CRLF sequence, or an empty string if no CRLF is found.
		 */
		std::string pull_encoded();
};

#endif
