#include "../include/BaseBlock.hpp"
#include "../include/CommonUtils.hpp"
#include "../include/Exceptions.hpp"

void BaseBlock::set_auto_index(const std::string& str)
{
	if (str.compare("on"))
		this->m_auto_index = true;
	else if (str.compare("off"))
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
	if (!this->m_index_set)
	{
		this->m_indexes.clear();	
		this->m_index_set = true;
	}
	this->m_pages_cache.insert(indexes.begin(), indexes.end());
	std::set<std::string>::iterator it_end = indexes.end();
	for (std::set<std::string>::iterator it = indexes.begin();
		it != it_end;
		it++)
	{
		const std::string* str_ptr = &(*this->m_pages_cache.find(*it));
		this->m_indexes.insert(str_ptr);
	}
}

ushort parse_http_code(const std::string& str)
{
	char* endptr;

	if (str.empty())
		throw WebservExceptions::InvalidValue();
	if (!std::isdigit(str[0]))
		throw WebservExceptions::InvalidValue();
	long code = strtol(str.c_str(), &endptr, 10);
	if (*endptr || errno == ERANGE)
		throw WebservExceptions::InvalidValue();
	if (code < 0 || code > 999)
		throw WebservExceptions::HttpCodeOutOfRange();
	return code;
}

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
		catch(const std::exception& e)
		{
			throw e;
		}
	}
}

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
			this->m_error_page[code] = str_ptr;
			this->m_set_redirect_pages.insert(code);
		}
		catch(const WebservExceptions::HttpCodeOutOfRange& e)
		{
			throw WebservExceptions::HttpRedirectCodeOutOfRange();
		}
		catch(const std::exception& e)
		{
			throw e;
		}
	}
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

const std::string& BaseBlock::get_index_page(const std::string& route) const
{
	std::string currentRoot = this->m_root;
}

const std::string& BaseBlock::get_error_page(ushort code) const
{
	std::map<ushort, const std::string*>::const_iterator it = this->m_error_page.find(code);
	struct stat statbuf;

	if (this->m_error_page.find(code) == this->m_error_page.end())
		throw WebservExceptions::NoAvailablePage();
	const std::string& str_ref = *it->second;
	if (str_ref[0] != '/')
		return (str_ref);

	std::string file_path = this->m_root;
	file_path.erase(file_path.size() - 1);
	file_path.append(str_ref);

	if (access(file_path.c_str(), F_OK))
		throw WebservExceptions::NoAvailablePage();
	if (access(file_path.c_str(), R_OK))
		throw WebservExceptions::ForbiddenAccess();
	if (stat(file_path.c_str(), &statbuf))
		throw WebservExceptions::NoAvailablePage();
	if (S_ISDIR(statbuf.st_mode))
		throw WebservExceptions::ForbiddenAccess();
	if (!S_ISREG(statbuf.st_mode))
		throw WebservExceptions::NonRegularFile();
	return (str_ref);
}

const std::string& BaseBlock::get_redirect_page(ushort code) const
{

}

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

BaseBlock::BaseBlock(BaseBlock& obj):
	m_auto_index(obj.m_auto_index),
	m_index_set(false),
	m_root(obj.m_root),
	m_client_max_body_size(obj.m_client_max_body_size),
	m_indexes(),
	m_set_error_pages(),
	m_set_redirect_pages(),
	m_error_page(),
	m_redirect_page(),
	m_pages_cache()
{

}