/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/24 19:23:21 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : BaseBlock(), 
                   m_locations(),
                   m_listen(),
                   m_server_names()
{}

Server::Server(const Server& other) : 
        BaseBlock(other),
        m_locations(other.m_locations),
        m_listen(other.m_listen),
        m_server_names(other.m_server_names)
{}

Server::Server(BaseBlock& baseBlock) :
        BaseBlock(baseBlock),
        m_locations(),
        m_listen(),
        m_server_names()
{}

Server::Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, std::string> >& listen,
               const std::set<std::string>& serverNames) :
    BaseBlock(baseBlock),
      m_locations(locations),
      m_listen(listen),
      m_server_names(serverNames)
{}

Server& Server::operator=(const Server& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(other);
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
    this->m_locations = locations;
}
void Server::add_location(Location& location)
{
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

void Server::add_listen(const std::pair<std::string, std::string>& entry)
{
    this->m_listen.push_back(entry);
}

void Server::remove_listen(std::pair<std::string, int>& listen)
{
    (void)listen; // Placeholder for future implementation
}

const std::vector<Location> Server::get_locations() const
{
    return (this->m_locations);
}

const std::vector<std::pair<std::string, std::string> > Server::get_listen() const
{
    return (this->m_listen);
}

const std::set<std::string> Server::get_server_names() const
{
    return (this->m_server_names);
}
