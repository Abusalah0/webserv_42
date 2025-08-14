/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:11:46 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/13 18:15:42 by abdsalah         ###   ########.fr       */
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
    if (this != &other)
    {
        BaseBlock::operator=(dynamic_cast<const BaseBlock&>(other));
        dir_listing = other.dir_listing;
        allowed_methods = other.allowed_methods;
        upload_path = other.upload_path;
        cgi_handlers = other.cgi_handlers;
    }
    return (*this);
}

Location::~Location()
{
    // Destructor logic if needed
}
