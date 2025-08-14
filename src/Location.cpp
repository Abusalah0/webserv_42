/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:11:46 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 11:31:59 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : 
    BaseBlock(),
    dir_listing(false)
{}

Location::Location(const Location& other) : 
    BaseBlock(dynamic_cast<const BaseBlock&>(other)),
    dir_listing(other.dir_listing),
    allowed_methods(other.allowed_methods),
    upload_path(other.upload_path),
    cgi_handlers(other.cgi_handlers)
{}

Location::Location(const BaseBlock& baseBlock,
        const std::set<std::string>& methods,
        const std::string& uploadPath,
        const std::map<std::string, std::string>& cgiHandlers) :
    BaseBlock(baseBlock),
    dir_listing(false),
    allowed_methods(methods),
    upload_path(uploadPath),
    cgi_handlers(cgiHandlers)
{}

Location& Location::operator=(const Location& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(dynamic_cast<const BaseBlock&>(other));
    dir_listing = other.dir_listing;
    allowed_methods = other.allowed_methods;
    upload_path = other.upload_path;
    cgi_handlers = other.cgi_handlers;
}

Location::~Location()
{
    // Destructor logic if needed
}

void Location::set_dir_listing(bool enabled)
{
    this->dir_listing = enabled;
}

bool Location::get_dir_listing() const
{
    return (this->dir_listing);
}

void Location::set_allowed_methods(const std::set<std::string>& methods)
{
    this->allowed_methods = methods;
}

const std::set<std::string>& Location::get_allowed_methods() const
{
    return (this->allowed_methods);
}

void Location::set_upload_path(const std::string& path)
{
    this->upload_path = path;
}

const std::string& Location::get_upload_path() const
{
    return (this->upload_path);
}

void Location::set_cgi_handlers(const std::map<std::string, std::string>& handlers)
{
    this->cgi_handlers = handlers;
}

const std::map<std::string, std::string>& Location::get_cgi_handlers() const
{
    return (this->cgi_handlers);
}

void Location::add_allowed_method(const std::string& method)
{
    this->allowed_methods.insert(method);
}

void Location::remove_allowed_method(const std::string& method)
{
    this->allowed_methods.erase(method);
}

void Location::add_cgi_handler(const std::string& extension, const std::string& handler)
{
    this->cgi_handlers.insert({extension, handler});
}

void Location::remove_cgi_handler(const std::string& extension)
{
    this->cgi_handlers.erase(extension);
}
