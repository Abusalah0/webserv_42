/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:11:46 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/03 23:36:39 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Location.hpp"

Location::Location() : 
    BaseBlock(),
    m_allowed_methods(),
    m_upload_path(),
    m_cgi_handlers()
{}

Location::Location(const BaseBlock& baseBlock) : 
    BaseBlock(baseBlock),
    m_allowed_methods(),
    m_upload_path(),
    m_cgi_handlers()
{}

Location::Location(const Location& other) : 
    BaseBlock(dynamic_cast<const BaseBlock&>(other)),
    m_allowed_methods(other.m_allowed_methods),
    m_upload_path(other.m_upload_path),
    m_cgi_handlers(other.m_cgi_handlers)
{}

Location::Location(const BaseBlock& baseBlock,
        const std::set<std::string>& methods,
        const std::string& uploadPath,
        const std::string& cgiHandlers) :
    BaseBlock(baseBlock),
    m_allowed_methods(methods),
    m_upload_path(uploadPath),
    m_cgi_handlers(cgiHandlers)
{}

Location& Location::operator=(const Location& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(dynamic_cast<const BaseBlock&>(other));
    m_allowed_methods = other.m_allowed_methods;
    m_upload_path = other.m_upload_path;
    m_cgi_handlers = other.m_cgi_handlers;
    return (*this);
}

Location::~Location()
{
    // Destructor logic if needed
}

void Location::set_allowed_methods(const std::set<std::string>& methods)
{
    this->m_allowed_methods.insert(methods.begin(), methods.end());
}

const std::set<std::string>& Location::get_allowed_methods() const
{
    return (this->m_allowed_methods);
}

void Location::set_upload_path(const std::string& path)
{
    this->m_upload_path = path;
}

const std::string& Location::get_upload_path() const
{
    return (this->m_upload_path);
}

void Location::set_cgi_handlers(const std::string& handlers)
{
    this->m_cgi_handlers = handlers;
}

const std::string& Location::get_cgi_handlers() const
{
    return (this->m_cgi_handlers);
}

void Location::add_allowed_method(const std::string& method)
{
    this->m_allowed_methods.insert(method);
}

void Location::remove_allowed_method(const std::string& method)
{
    this->m_allowed_methods.erase(method);
}

bool Location::is_method_allowed(const std::string& method) const
{
	std::set<std::string>::iterator it = this->m_allowed_methods.find(method);
	if (it == this->m_allowed_methods.end())
		return true;
	return false;
}

void Location::set_upload_store(const std::string& path)
{
    this->m_upload_store = path;
}

const std::string& Location::get_upload_store() const
{
    return (this->m_upload_store);
}
