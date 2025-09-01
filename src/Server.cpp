/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/01 19:10:38 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <Exceptions.hpp>
#include <stdexcept>

Server::Server() : BaseBlock(),
					m_is_default(false),
					m_root_location_exist(false),
                	m_locations(),
                	m_listen(),
                	m_server_names()
{}

Server::Server(const Server& other) : 
        BaseBlock(other),
		m_is_default(other.m_is_default),
        m_locations(other.m_locations),
        m_listen(other.m_listen),
        m_server_names(other.m_server_names)
{}

Server::Server(BaseBlock& baseBlock) :
        BaseBlock(baseBlock),
		m_is_default(false),
		m_root_location_exist(false),
        m_locations(),
        m_listen(),
        m_server_names()
{}

Server::Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, std::string> >& listen,
               const std::set<std::string>& serverNames) :
    BaseBlock(baseBlock),
		m_is_default(),
    	m_locations(locations),
    	m_listen(listen),
    	m_server_names(serverNames)
{}

Server& Server::operator=(const Server& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(other);
	this->m_is_default = other.m_is_default;
    this->m_locations = other.m_locations;
    this->m_listen = other.m_listen;
    this->m_server_names = other.m_server_names;
    return (*this);
}

Server::~Server()
{
}

void Server::set_locations(std::vector<Location>& locations)
{
    this->m_locations.insert(this->m_locations.end(), locations.begin(), locations.end());
}

void Server::add_location(Location& location)
{
	if (location.get_upload_path() == "/")
	{
		this->m_root_location_exist = true;
		this->m_root_location_index = this->m_locations.size();
	}
    this->m_locations.push_back(location);
}

void Server::remove_location(Location& location)
{
    (void)location; // Placeholder for future implementation
}

void Server::set_server_names(std::set<std::string>& names)
{
    this->m_server_names.insert(names.begin(), names.end());
}

void Server::add_server_name(std::string& name)
{
    this->m_server_names.insert(name);
}

//void Server::remove_server_name(std::string& name)
//{
//    this->m_server_names.erase(name);
//}

void Server::set_listen(std::vector<std::pair<std::string, std::string> >& listen)
{
    this->m_listen = listen;
}

void Server::add_listen(std::pair<std::string, std::string>& listen)
{
    this->m_listen.push_back(listen);
}

void Server::remove_listen(std::pair<std::string, std::string>& listen)
{
    (void)listen; // Placeholder for future implementation
}

const std::vector<Location> Server::get_locations() const
{
    return (this->m_locations);
}

bool Server::match_virtual_host(const std::string &virtual_host) const
{
	if (!this->m_server_names.size())
		return true;
    if (this->m_server_names.find(virtual_host) == this->m_server_names.end())
	{
        return false;
	}
	return true;
}

// Location &Server::get_location_by_path(const std::string& path, const std::string &virtual_host) const
// {
//     // For now, return the first location or throw an exception if none exist
//     if (this->m_locations.empty())
//         throw std::runtime_error("No locations available");

//     match_virtual_host(virtual_host);
//     std::string normalized = normalize_path(path);

//     // lets match the normalized path with the locations
//     for (std::vector<Location>::const_iterator it = this->m_locations.begin(); it != this->m_locations.end(); ++it)
//     {
//         if (it->get_root() == normalized)
//             return (const_cast<Location&>(*it));
//     }
//     throw std::runtime_error("No matching location found");
// }

const Location& Server::match_location(std::string& route) const
{
	for (size_t i = 0; i < this->m_locations.size(); i++)
	{
		if (this->m_locations[i].get_upload_path() == route)
			return this->m_locations[i];
	}
	if (this->m_root_location_exist)
		return this->m_locations[this->m_root_location_index];
	throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
}

const std::vector<std::pair<std::string, std::string> >& Server::get_listen() const
{
    return (this->m_listen);
}

const std::set<std::string> Server::get_server_names() const
{
    return (this->m_server_names);
}

// void Server::match_location(const std::string &path) const
// {
//     std::string normalized = normalize_path(path);
//     for (size_t i = 0; i < this->m_locations.size(); i++)
//     {
//         if (this->m_locations[i].get_root() == normalized)
//         {
            
//             return ;
//         }
//     }
// }

void Server::set_default_server()
{
	this->m_is_default = true;
}

bool Server::is_default_server()
{
	return this->m_is_default;
}