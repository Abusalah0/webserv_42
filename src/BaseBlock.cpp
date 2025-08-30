#include "../include/BaseBlock.hpp"
#include "../include/CommonUtils.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

/**
 * Setter for autoindex
 * @param str autoindex status in string format
 */
void BaseBlock::set_auto_index(const std::string& str)
{
	if (!str.compare("on"))
		this->m_auto_index = true;
	else if (!str.compare("off"))
		this->m_auto_index = false;
	else
		throw WebservExceptions::InvalidValue();
}

/**
 * Setter for root
 * @param root root path
 */
void BaseBlock::set_root(const std::string& root)
{
	this->m_root.clear();
    if (root.empty() || root[0] != '/')
        this->m_root = ROOT_PREFIX;
    this->m_root.append(root);
    if (str_back(root) == '/')
        this->m_root.erase(this->m_root.size() - 1);
}

/**
 * Setter for client_max_body_size
 * @param str_size client_max_body_size in string format
 */
void BaseBlock::set_client_max_body_size(const std::string& str_size)
{
	char sizeCategory = 0;
    char *endptr;
	const char *excpected_endptr;

	if (str_size.empty() || !isdigit(str_size[0]))
		throw WebservExceptions::InvalidValue();
    if (str_size.find('.') != std::string::npos)
	{
        throw WebservExceptions::InvalidValue();
	}
	excpected_endptr = str_size.c_str() + str_size.size();
    if (!isdigit(str_back(str_size)))
	{
        sizeCategory = tolower(str_back(str_size));
		--excpected_endptr;
	}
	errno = 0;
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

/**
 * Insert index entries
 * @param indexes Vector of indexes
 */
void BaseBlock::insert_index_pages(const std::vector<std::string>& indexes)
{
	if (!this->m_index_set)
	{
		this->m_indexes.clear();
		this->m_index_set = true;
	}
	this->m_pages_cache.insert(indexes.begin(), indexes.end());
	for (size_t i = 0; i < indexes.size(); i++)
	{
		const std::string* str_ptr = &(*this->m_pages_cache.find(indexes[i]));
		this->m_indexes.push_back(str_ptr);
	}
}

/**
 * Insert error page for specified codes
 * @param codes Vector of codes
 */
void BaseBlock::insert_error_page(const std::set<std::string>& codes, const std::string& page)
{
	std::set<std::string>::iterator it_end = codes.end();
	const std::string* str_ptr = &(*this->m_pages_cache.insert(page).first);

	for (std::set<std::string>::iterator it = codes.begin();
		it != it_end;
		it++)
	{
		try
		{
			ushort code = parse_http_code(*it);
			if (code < 300 || code > 599)
				throw WebservExceptions::HttpErrorCodeOutOfRange();
			if (this->m_set_error_pages.find(code) != this->m_set_error_pages.end())
				continue;
			this->m_error_page[code] = str_ptr;
			this->m_set_error_pages.insert(code);
		}
		catch(const WebservExceptions::HttpCodeOutOfRange& e)
		{
			throw WebservExceptions::HttpErrorCodeOutOfRange();
		}
		catch(...)
		{
			throw;
		}
	}
}

/**
 * Insert redirect page for specified codes
 * @param codes Vector of codes
 */
void BaseBlock::insert_redirect_page(const std::set<std::string>& codes, const std::string& page)
{
	std::set<std::string>::iterator it_end = codes.end();
	const std::string* str_ptr = &(*this->m_pages_cache.insert(page).first);

	for (std::set<std::string>::iterator it = codes.begin();
		it != it_end;
		it++)
	{
		try
		{
			ushort code = parse_http_code(*it);
			if (code < 300 || code > 399)
				throw WebservExceptions::HttpRedirectCodeOutOfRange();
			if (this->m_set_redirect_pages.find(code) != this->m_set_redirect_pages.end())
				continue;
			this->m_redirect_page[code] = str_ptr;
			this->m_set_redirect_pages.insert(code);
		}
		catch(const WebservExceptions::HttpCodeOutOfRange& e)
		{
			throw WebservExceptions::HttpRedirectCodeOutOfRange();
		}
		catch(...)
		{
			throw;
		}
	}
}

/**
 * Getter for autoindex
 * @return autoindex status
 */
bool BaseBlock::get_auto_index() const
{
	return this->m_auto_index;
}

/**
 * Getter for root
 * @return root absolute path
 */
const std::string& BaseBlock::get_root() const
{
	return this->m_root;
}

/**
 * Getter for client_max_body_size
 * @return client_max_body_size
 */
std::size_t BaseBlock::get_client_max_body_size() const
{
	return this->m_client_max_body_size;
}

/**
 * Getter for index pages
 * @return vector of index pages
 */
std::vector<std::string> BaseBlock::get_index_pages() const
{
	std::vector<std::string> index_pages;
	for (size_t i = 0; i < this->m_indexes.size(); i++)
	{
		index_pages.push_back(*this->m_indexes[i]);
	}
	return index_pages;
}

/**
 * Getter for redirect pages
 * @return map of redirect pages with their codes
 */
std::map<ushort, std::string> BaseBlock::get_redirect_pages() const
{
	std::map<ushort, std::string> redirect_pages;
	for (std::map<ushort, const std::string*>::const_iterator it = this->m_redirect_page.begin();
		it != this->m_redirect_page.end();
		++it)
	{
		redirect_pages[it->first] = *(it->second);
	}
	return redirect_pages;
}

/**
 * Getter for error pages
 * @return map of error pages with their codes
 */
std::map<ushort, std::string> BaseBlock::get_error_pages() const
{
	std::map<ushort, std::string> error_pages;
	for (std::map<ushort, const std::string*>::const_iterator it = this->m_error_page.begin();
		it != this->m_error_page.end();
		++it)
	{
		error_pages[it->first] = *(it->second);
	}
	return error_pages;
}

/**
 * Initiate index search and gets the first index available
 * @param[in] route
 * @return index_page absolute path
 */
std::string BaseBlock::get_index_page(const std::string& route) const
{
	if (this->m_indexes.empty())
		throw WebservExceptions::NoAvailablePage();

	std::string current_root = this->m_root;
	current_root.append(route);
	if (str_back(current_root) != '/')
		current_root.push_back('/');
	struct stat statbuf;
	for (size_t i = 0; i < this->m_indexes.size(); i++)
	{
		std::string file_path;
		const std::string* str_ptr = this->m_indexes[i];
		if (this->m_indexes[i]->at(0) == '/')
		{
			file_path.append(this->m_root);
			file_path.append(*str_ptr);
			return file_path;
		}
		else
		{
			file_path.append(current_root);
			file_path.append(*str_ptr);
		}
		if (stat(file_path.c_str(), &statbuf))
		{
			if (errno == ENOTDIR)
				throw WebservExceptions::NoAvailablePage();
			continue;
		}
		if (access(file_path.c_str(), R_OK))
			throw WebservExceptions::ForbiddenAccess();
		if (S_ISDIR(statbuf.st_mode))
		{
			if (str_back(file_path) != '/')
				file_path.push_back('/');
			return file_path;
		}
		if (S_ISREG(statbuf.st_mode))
			return file_path;
		throw WebservExceptions::NonRegularFile();
	}
	throw WebservExceptions::NoAvailablePage();
}

/**
 * Gets error page for specific code if available
 * @param[in] code
 * @return error_page absolute path
 */
std::string BaseBlock::get_error_page(ushort code) const
{
	std::map<ushort, const std::string*>::const_iterator it = this->m_error_page.find(code);
	struct stat statbuf;

	if (it == this->m_error_page.end())
		throw WebservExceptions::NoAvailablePage();
	const std::string& str_ref = *it->second;
	if (str_ref[0] != '/')
		return (str_ref);

	std::string file_path = this->m_root;
	file_path.append(str_ref);

	if (stat(file_path.c_str(), &statbuf))
		throw WebservExceptions::NoAvailablePage();
	if (access(file_path.c_str(), R_OK))
		throw WebservExceptions::ForbiddenAccess();
	if (S_ISDIR(statbuf.st_mode))
		throw WebservExceptions::ForbiddenAccess();
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::NonRegularFile();
	return (file_path);
}

/**
 * Gets redirect page for specific code if available
 * @param[in] code
 * @return redirect_page absolute path
 */
std::string BaseBlock::get_redirect_page(ushort code) const
{
	std::map<ushort, const std::string*>::const_iterator it = this->m_redirect_page.find(code);
	struct stat statbuf;

	if (it == this->m_redirect_page.end())
		throw WebservExceptions::NoAvailablePage();
	const std::string& str_ref = *it->second;
	if (str_ref[0] != '/')
		return (str_ref);

	std::string file_path = this->m_root;
	file_path.append(str_ref);

	if (stat(file_path.c_str(), &statbuf))
		throw WebservExceptions::NoAvailablePage();
	if (access(file_path.c_str(), R_OK))
		throw WebservExceptions::ForbiddenAccess();
	if (S_ISDIR(statbuf.st_mode))
		throw WebservExceptions::ForbiddenAccess();
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::NonRegularFile();
	return (file_path);
}

/**
 * BaseBlock default constructor
 * @return BaseBlock
 */
BaseBlock::BaseBlock():
	m_auto_index(false),
	m_index_set(false),
	m_root(DEFAULT_ROOT_PATH),
	m_client_max_body_size(MEGABYTE),
	m_indexes(),
	m_set_error_pages(),
	m_set_redirect_pages(),
	m_error_page(),
	m_redirect_page(),
	m_pages_cache()
{}

/**
 * BaseBlock copy constructor
 * @param[in] BaseBlock
 * @return Copied BaseBlock
 */
BaseBlock::BaseBlock(const BaseBlock& obj):
	m_auto_index(obj.m_auto_index),
	m_index_set(false),
	m_root(obj.m_root),
	m_client_max_body_size(obj.m_client_max_body_size),
	m_indexes(),
	m_set_error_pages(),
	m_set_redirect_pages(),
	m_error_page(),
	m_redirect_page(),
	m_pages_cache(obj.m_pages_cache)
{
	for (size_t i = 0; i < obj.m_indexes.size(); i++)
	{
		std::set<std::string>::iterator it_str = this->m_pages_cache.find(*obj.m_indexes[i]);
		this->m_indexes.push_back(&(*it_str));
	}
	for (std::map<ushort, const std::string*>::const_iterator it = obj.m_error_page.begin();
		it != obj.m_error_page.end();
		it++)
	{
		std::set<std::string>::iterator it_str = this->m_pages_cache.find(*(*it).second);
		this->m_error_page[(*it).first] = &(*it_str);
	}
	for (std::map<ushort, const std::string*>::const_iterator it = obj.m_redirect_page.begin();
		it != obj.m_redirect_page.end();
		it++)
	{
		std::set<std::string>::iterator it_str = this->m_pages_cache.find(*(*it).second);
		this->m_redirect_page[(*it).first] = &(*it_str);
	}
}

BaseBlock::~BaseBlock() {}
