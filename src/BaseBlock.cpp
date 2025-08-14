#include "../include/BaseBlock.hpp"
#include "../include/CommonUtils.hpp"
#include "../include/Exceptions.hpp"

void BaseBlock::set_auto_index(const std::string& str)
{
	if (!str.compare("on"))
		this->m_auto_index = true;
	else if (!str.compare("off"))
		this->m_auto_index = false;
	else
		throw WebservExceptions::InvalidValue();
}

void BaseBlock::set_root(const std::string& root)
{
	this->m_root.clear();
    if (root.empty() || root[0] != '/')
        this->m_root = ROOT_PREFIX;
    this->m_root.append(root);
    if (str_back(root) != '/')
        this->m_root.push_back('/');
}

void BaseBlock::set_client_max_body_size(const std::string& str_size)
{
	char sizeCategory = 0;
    char *endptr;
	const char *excpected_endptr;

	if (str_size.empty())
		throw WebservExceptions::InvalidValue();
    if (str_size.find('.') != std::string::npos)
        throw WebservExceptions::InvalidValue();
	excpected_endptr = str_size.c_str() + str_size.size();
    if (!isdigit(str_back(str_size)))
	{
        sizeCategory = tolower(str_back(str_size));
		--excpected_endptr;
	}
    this->m_client_max_body_size = strtoul(str_size.c_str(), &endptr, 10);
    if (endptr != excpected_endptr || errno == ERANGE)
        throw WebservExceptions::InvalidValue();
    switch (sizeCategory)
    {
    	case 0:
    	    return;
    	case 'k':
    	    if (this->m_client_max_body_size > MAX_KILOBYTE)
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= KILOBYTE;
    	    return;
    	case 'm':
    	    if (this->m_client_max_body_size > MAX_MEGABYTE)
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= MEGABYTE;
    	    return;
    	case 'g':
    	    if (this->m_client_max_body_size > MAX_GIGABYTE)
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= GIGABYTE;
    	    return;
    	default:
    	    throw WebservExceptions::InvalidValue();
    }
}

void BaseBlock::insert_index_pages(const std::set<std::string>& indexes)
{
	this->m_pages_cache.insert(indexes.cbegin(), indexes.cend());
	std::set<std::string>::const_iterator it_end = indexes.cend();
	for (std::set<std::string>::const_iterator it = indexes.cbegin();
		it != it_end;
		it++)
	{
		const std::string* str_ptr = &(*this->m_pages_cache.find(*it));
		this->m_indexes.insert(str_ptr);
	}
}

void BaseBlock::insert_error_page(uint32_t code, const std::string& page)
{
	if (this->m_error_page.find(code) == this->m_error_page.cend())
		return;
	this->m_pages_cache.insert(page);
	const std::string* str_ptr = &(*this->m_pages_cache.find(page));
	this->m_error_page[code] = str_ptr;
}

void BaseBlock::insert_redirect_page(uint32_t code, const std::string& page)
{
	if (this->m_redirect_page.find(code) == this->m_redirect_page.cend())
		return;
	this->m_pages_cache.insert(page);
	const std::string* str_ptr = &(*this->m_pages_cache.find(page));
	this->m_redirect_page[code] = str_ptr;
}

bool BaseBlock::get_auto_index() const
{
	return this->m_auto_index;
}

const std::string& BaseBlock::get_root() const
{
	return this->m_root;
}

std::size_t BaseBlock::get_client_max_body_size() const
{
	return this->m_client_max_body_size;
}

const std::string& BaseBlock::get_index_page() const
{
	
}

const std::string& BaseBlock::get_error_page(uint32_t code) const
{

}

const std::string& BaseBlock::get_redirect_page(uint32_t code) const
{

}

BaseBlock::BaseBlock():
	m_auto_index(false),
	m_root(DEFAULT_ROOT_PATH),
	m_client_max_body_size(MEGABYTE),
	m_indexes(),
	m_error_page(),
	m_redirect_page(),
	m_pages_cache()
{}

BaseBlock::BaseBlock(const BaseBlock& obj):
	m_auto_index(obj.m_auto_index),
	m_root(obj.m_root),
	m_client_max_body_size(obj.m_client_max_body_size)
{
	// Copy other members if needed
}