/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:09:36 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/19 02:17:03 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASEBLOCK_HPP
#define BASEBLOCK_HPP

#include <map>
#include <set>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include "CommonUtils.hpp"

/**
 * @brief Structure to represent an index entry, which can be a file or a directory.
 */
struct IndexEntry
{
	bool is_dir;
	std::string path;
};

class BaseBlock
{
	private:
		bool m_auto_index; // to check if autoindex is on or off
		bool m_index_set;// to check if index is set by user
		std::string m_root;// the root directory
		std::size_t m_client_max_body_size;// the size in bytes
		std::vector<const std::string*> m_indexes;
		std::set<ushort> m_set_error_pages;// to avoid duplicate error codes
		std::map<ushort, const std::string*> m_error_page;// map of error code to error page
		std::set<std::string> m_pages_cache;
	public:
		/**
		* @brief Sets autoindex based on the value of str.
		* @param str String to convert to boolean
		*/
		void set_auto_index(const std::string& str);
		/**
		* @brief Sets root directory.
		* @param str Root directory path
		*/
		void set_root(const std::string& root);
		/**
		* @brief Sets client max body size.
		* @param str_size Client max body size
		*/
		void set_client_max_body_size(const std::string& str_size);
		/**
		* @brief Inserts index pages.
		* @param indexes Vector of index page paths
		*/
		void insert_index_pages(const std::vector<std::string>& indexes);
		/**
		* @brief Inserts error pages.
		* @param codes Set of HTTP error codes
		* @param pages Error page path
		*/
		void insert_error_page(const std::set<std::string>& codes, const std::string& pages);
		/**
		 * @brief Checks if autoindex is enabled
		 * @return true if autoindex is enabled, false otherwise
		*/
		bool get_auto_index() const;
		/**
		 * @brief Gets the root directory.
		 * @return The root directory path.
		 */
		const std::string& get_root() const;
		/**
		 * @brief Gets the client max body size.
		 * @return The client max body size.
		 */
		std::size_t get_client_max_body_size() const;
		/**
		 * @brief Gets the index pages.
		 * @return A vector of index page paths.
		 */
		std::vector<std::string> get_index_pages() const;
		/**
		 * @brief Gets the set of error pages.
		 * @return A map of HTTP error codes to their corresponding error page paths.
		 */
		std::map<ushort, std::string> get_error_pages() const;
		IndexEntry get_index_page(const std::string& route) const;
		/**
		 * @brief Gets the error page for a given HTTP code.
		 * @param code The HTTP error code.
		 * @return The error page path if it exists, an empty string otherwise.
		 */
		std::string get_error_page(ushort code) const;
		/**
		 * @brief Default constructor, copy constructor, and destructor.
		 */
		BaseBlock();
		BaseBlock(const BaseBlock& obj);
		virtual ~BaseBlock();
};

#endif