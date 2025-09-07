/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:11:46 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/07 18:41:31 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/Location.hpp"
#include "../include/Exceptions.hpp"

Location::Location() : 
    BaseBlock(),
    m_allowed_methods(),
    m_upload_path(),
    m_cgi_extension(),
	m_cgi_pass()
{}

Location::Location(const BaseBlock& baseBlock) : 
    BaseBlock(baseBlock),
    m_allowed_methods(),
    m_upload_path(),
    m_cgi_extension(),
	m_cgi_pass()
{}

Location::Location(const Location& other) : 
    BaseBlock(dynamic_cast<const BaseBlock&>(other)),
    m_allowed_methods(other.m_allowed_methods),
    m_upload_path(other.m_upload_path),
	m_cgi_extension(other.m_cgi_extension),
	m_cgi_pass(other.m_cgi_pass)
{}

Location::Location(const BaseBlock& baseBlock,
        const std::set<std::string>& methods,
        const std::string& uploadPath,
        const std::string& cgiExtension,
		const std::string& cgiPass) :
    BaseBlock(baseBlock),
    m_allowed_methods(methods),
    m_upload_path(uploadPath),
    m_cgi_extension(cgiExtension),
	m_cgi_pass(cgiPass)
{}

Location& Location::operator=(const Location& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(dynamic_cast<const BaseBlock&>(other));
    this->m_allowed_methods = other.m_allowed_methods;
    this->m_upload_path = other.m_upload_path;
    this->m_cgi_extension = other.m_cgi_extension;
	this->m_cgi_pass = other.m_cgi_pass;
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
	if (str_back(this->m_upload_path) != '/')
		this->m_upload_path.push_back('/');
}

const std::string& Location::get_upload_path() const
{
    return (this->m_upload_path);
}

void Location::set_cgi_extension(const std::string& extension)
{
	this->m_cgi_extension = extension;
}

const std::string& Location::get_cgi_extension() const
{
	return this->m_cgi_extension;
}

void Location::set_cgi_pass(const std::string& cgi_pass)
{
	this->m_cgi_pass = cgi_pass;
}

const std::string& Location::get_cgi_pass() const
{
	return this->m_cgi_pass;
}

bool Location::is_cgi_requirments(const std::string& target) const
{
	if (this->m_cgi_pass.empty())
		return false;
	if (this->m_cgi_extension.empty())
		return true;
	size_t pos = target.rfind(".");
	if (pos == std::string::npos)
		return false;
	std::string ext = target.substr(pos);
	if (ext == this->m_cgi_extension)
		return true;
	return false;
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
	// If no methods are specified, all methods are allowed
	if (this->m_allowed_methods.empty())
		return true;
	
	std::set<std::string>::const_iterator it = this->m_allowed_methods.find(method);
	// Return true if method is found in allowed methods
	return (it != this->m_allowed_methods.end());
}

// void Location::set_upload_store(const std::string& path)
// {
//     this->m_upload_store = path;
// }

// const std::string& Location::get_upload_store() const
// {
//     return (this->m_upload_store);
// }
