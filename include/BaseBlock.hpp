/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:09:36 by amsaleh           #+#    #+#             */
/*   Updated: 2025/08/13 14:10:18 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASEBLOCK_HPP
#define BASEBLOCK_HPP

#include <string>
#include <map>
#include <set>
#include <cstdint>

class BaseBlock
{
	private:
		bool m_auto_index;
		std::string m_root;
		std::size_t m_client_max_body_size;
		std::set<const std::string*> m_indexes;
		std::map<uint32_t, const std::string*> m_error_page;
		std::map<uint32_t, const std::string*> m_redirect_page;
		std::set<const std::string> m_pages_cache;
	public:
		void set_auto_index(const std::string& str);
		void set_root(const std::string& root);
		void set_client_max_body_size(const std::string& str_size);
		void insert_index_pages(const std::set<std::string>& indexes);
		void insert_error_page(uint32_t code, const std::string& pages);
		void insert_redirect_page(uint32_t code, const std::string& pages);
		bool get_auto_index() const;
		const std::string& get_root() const;
		std::size_t get_client_max_body_size() const;
		const std::string& get_index_page() const;
		const std::string& get_error_page(uint32_t code) const;
		const std::string& get_redirect_page(uint32_t code) const;
		BaseBlock();
		BaseBlock(BaseBlock& obj);
		virtual ~BaseBlock();
};

#endif