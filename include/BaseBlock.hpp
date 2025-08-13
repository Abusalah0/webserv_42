#ifndef BASEBLOCK_HPP
#define BASEBLOCK_HPP

#include <string>
#include <map>
#include <set>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

class BaseBlock
{
	private:
		bool m_auto_index;
		bool m_index_set;
		std::string m_root;
		std::size_t m_client_max_body_size;
		std::set<const std::string*> m_indexes;
		std::set<ushort> m_set_error_pages;
		std::set<ushort> m_set_redirect_pages;
		std::map<ushort, const std::string*> m_error_page;
		std::map<ushort, const std::string*> m_redirect_page;
		std::set<const std::string> m_pages_cache;
	public:
		void set_auto_index(const std::string& str);
		void set_root(const std::string& root);
		void set_client_max_body_size(const std::string& str_size);
		void insert_index_pages(const std::set<std::string>& indexes);
		void insert_error_page(const std::set<std::string>& codes, const std::string& pages);
		void insert_redirect_page(const std::set<std::string>& codes, const std::string& pages);
		bool get_auto_index() const;
		const std::string& get_root() const;
		std::size_t get_client_max_body_size() const;
		const std::string& get_index_page(const std::string& route) const;
		const std::string& get_error_page(ushort code) const;
		const std::string& get_redirect_page(ushort code) const;
		BaseBlock();
		BaseBlock(BaseBlock& obj);
		virtual ~BaseBlock();
};

#endif