/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:17:11 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/20 14:19:33 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/BaseBlock.hpp"
#include "../include/CommonUtils.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

void BaseBlock::set_auto_index(const std::string& str)
{
	if (!str.compare("on"))
	{
		this->m_auto_index = true;
	}
	else if (!str.compare("off"))
	{
		this->m_auto_index = false;
	}
	else
		throw WebservExceptions::InvalidValue();
}

void BaseBlock::set_root(const std::string& root)
{
	this->m_root.clear();
    if (root.empty() || root[0] != '/')
	{
        this->m_root = ROOT_PREFIX;
	}
	
    this->m_root.append(root);
    if (str_back(root) == '/')
	{
        this->m_root.erase(this->m_root.size() - 1);
	}
}

void BaseBlock::set_client_max_body_size(const std::string& str_size)
{
	char sizeCategory = 0;
    char *endptr;
	const char *excpected_endptr;

	if (str_size.empty() || !std::isdigit(str_size[0]))
		throw WebservExceptions::InvalidValue();
    if (str_size.find('.') != std::string::npos)// decimal point found, not an integer
	{
        throw WebservExceptions::InvalidValue();
	}
	
	excpected_endptr = str_size.c_str() + str_size.size();
    if (!std::isdigit(str_back(str_size)))
	{
        sizeCategory = tolower(str_back(str_size));
		--excpected_endptr;
	}
	
	errno = 0;
    this->m_client_max_body_size = std::strtoul(str_size.c_str(), &endptr, 10);
    if (endptr != excpected_endptr || errno == ERANGE)// not fully converted or out of range
	{
        throw WebservExceptions::InvalidValue();
	}
		
    switch (sizeCategory)
    {
    	case 0:// no size category
    	    return ;
    	case 'k':// kilobyte
    	    if (this->m_client_max_body_size > MAX_KILOBYTE)// to prevent overflow
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= KILOBYTE;
    	    return ;
    	case 'm':// megabyte
    	    if (this->m_client_max_body_size > MAX_MEGABYTE)
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= MEGABYTE;
    	    return ;
    	case 'g':// gigabyte
    	    if (this->m_client_max_body_size > MAX_GIGABYTE)
    	        throw WebservExceptions::InvalidValue();
    	    this->m_client_max_body_size *= GIGABYTE;
    	    return ;
    	default:// invalid size category
    	    throw WebservExceptions::InvalidValue();
    }
}

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
		const std::string* str_ptr = &(*this->m_pages_cache.find(indexes[i]));// get pointer to the string in cache
		this->m_indexes.push_back(str_ptr);
	}
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
				continue ;

			this->m_error_page[code] = str_ptr;
			this->m_set_error_pages.insert(code);
		}
		catch(const WebservExceptions::HttpCodeOutOfRange& e)
		{
			throw WebservExceptions::HttpErrorCodeOutOfRange();
		}
		catch(...)
		{
			throw ;
		}
	}
}

bool BaseBlock::get_auto_index() const
{
	return (this->m_auto_index);
}

const std::string& BaseBlock::get_root() const
{
	return (this->m_root);
}

std::size_t BaseBlock::get_client_max_body_size() const
{
	return (this->m_client_max_body_size);
}

std::vector<std::string> BaseBlock::get_index_pages() const
{
	std::vector<std::string> index_pages;
	
	for (size_t i = 0; i < this->m_indexes.size(); i++)
	{
		index_pages.push_back(*this->m_indexes[i]);
	}
	
	return (index_pages);
}

std::map<ushort, std::string> BaseBlock::get_error_pages() const
{
	std::map<ushort, std::string> error_pages;

	for (std::map<ushort, const std::string*>::const_iterator it = this->m_error_page.begin();
		it != this->m_error_page.end();
		++it)
	{
		error_pages[it->first] = *(it->second);
	}
	
	return (error_pages);
}

IndexEntry BaseBlock::get_index_page(const std::string& route) const
{
	if (this->m_indexes.empty())
	{
		throw WebservExceptions::NoAvailablePage();
	}

	IndexEntry entry;
	entry.is_dir = false;
	std::string current_root = this->m_root;
	current_root.append(route);

	if (str_back(current_root) != '/')
		current_root.push_back('/');

	struct stat statbuf;

	for (size_t i = 0; i < this->m_indexes.size(); i++)
	{
		const std::string* str_ptr = this->m_indexes[i];
		entry.path.clear();
		
		if (this->m_indexes[i]->at(0) == '/')// absolute path
		{
			entry.path.append(this->m_root);
			entry.path.append(*str_ptr);
			return (entry);
		}
		else // relative path
		{
			entry.path.append(current_root);
			entry.path.append(*str_ptr);
		}
		
		if (stat(entry.path.c_str(), &statbuf))// file does not exist
		{
			if (errno == ENOTDIR)
				throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
			continue ;
		}
		
		if (access(entry.path.c_str(), R_OK))// no read permission
		{
			throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
		}
			
		if (S_ISDIR(statbuf.st_mode))// is a directory
		{
			entry.is_dir = true;
			if (str_back(entry.path) != '/')// append (/) at the end of directory names
				entry.path.push_back('/');
			return (entry);
		}
		if (S_ISREG(statbuf.st_mode))// is a regular file
			return (entry);
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
	
	throw WebservExceptions::NoAvailablePage();
}

std::string BaseBlock::get_error_page(ushort code) const
{
	std::map<ushort, const std::string*>::const_iterator it;
	struct stat statbuf;

	it = this->m_error_page.find(code);// find the error page for the given code
	if (it == this->m_error_page.end())// not found
	{
		throw WebservExceptions::NoAvailablePage();
	}

	const std::string& str_ref = *it->second;// dereference to get the string
	if (str_ref[0] != '/')// relative path
		return (str_ref);

	std::string file_path = this->m_root;
	file_path.append(str_ref);

	if (stat(file_path.c_str(), &statbuf))// file does not exist
	{
		throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
	}
	if (access(file_path.c_str(), R_OK))// no read permission
	{
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
	if (S_ISDIR(statbuf.st_mode))// is a directory
	{
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
	if (!S_ISREG(statbuf.st_mode))// not a regular file
	{
		throw WebservExceptions::HTTPException(HTTP_FORBIDDEN);
	}
	
	return (file_path);
}

BaseBlock::BaseBlock():
	m_auto_index(false),
	m_index_set(false),
	m_root(DEFAULT_ROOT_PATH),
	m_client_max_body_size(MEGABYTE),
	m_indexes(),
	m_set_error_pages(),
	m_error_page(),
	m_pages_cache()
{}

BaseBlock::BaseBlock(const BaseBlock& obj):
	m_auto_index(obj.m_auto_index),
	m_index_set(false),
	m_root(obj.m_root),
	m_client_max_body_size(obj.m_client_max_body_size),
	m_indexes(),
	m_set_error_pages(),
	m_error_page(),
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
}

BaseBlock::~BaseBlock() {}
