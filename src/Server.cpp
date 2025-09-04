/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/04 19:24:00 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Exceptions.hpp"
#include <stdexcept>

Server::Server() : BaseBlock(),
					m_is_default(false),
					m_root_location_exist(false),
					m_root_location_index(0),
                	m_locations(),
                	m_listen(),
                	m_server_names()
{}

Server::Server(const Server& other) : 
        BaseBlock(other),
		m_is_default(other.m_is_default),
		m_root_location_exist(other.m_root_location_exist),
		m_root_location_index(other.m_root_location_index),
        m_locations(other.m_locations),
        m_listen(other.m_listen),
        m_server_names(other.m_server_names)
{}

Server::Server(BaseBlock& baseBlock) :
        BaseBlock(baseBlock),
		m_is_default(false),
		m_root_location_exist(false),
		m_root_location_index(0),
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

const Location& Server::match_location(std::string& route) const
{
	const Location* best_match = NULL;
	size_t best_match_length = 0;
	
	for (size_t i = 0; i < this->m_locations.size(); i++)
	{
		const std::string& location_path = this->m_locations[i].get_upload_path();
		
		// Check if route starts with location_path (prefix matching)
		if (route.find(location_path) == 0)
		{
			// For exact match or if location_path is longer than current best match
			if (location_path.length() > best_match_length)
			{
				best_match = &this->m_locations[i];
				best_match_length = location_path.length();
			}
		}
	}
	
	if (best_match)
		return *best_match;
		
	throw WebservExceptions::LocationNotFound();
}

const std::vector<std::pair<std::string, std::string> >& Server::get_listen() const
{
    return (this->m_listen);
}

const std::set<std::string> Server::get_server_names() const
{
    return (this->m_server_names);
}

void Server::set_default_server()
{
	this->m_is_default = true;
}

bool Server::is_default_server() const
{
	return this->m_is_default;
}

bool Server::root_location_exist() const
{
	return this->m_root_location_exist;
}

const Location& Server::get_root_location() const
{
	return this->m_locations[this->m_root_location_index];
}