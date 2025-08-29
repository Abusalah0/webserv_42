/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/29 11:54:11 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : BaseBlock(), 
                   locations(),
                   listen(),
                   server_names()
{}

Server::Server(const Server& other) : 
        BaseBlock(other),
        locations(other.locations),
        listen(other.listen),
        server_names(other.server_names)
{}

Server::Server(BaseBlock& baseBlock) :
        BaseBlock(baseBlock),
        locations(),
        listen(),
        server_names()
{}

Server::Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, int> >& listen,
               const std::set<std::string>& serverNames) :
    BaseBlock(baseBlock),
      locations(locations),
      listen(listen),
      server_names(serverNames)
{}

Server& Server::operator=(const Server& other)
{
    if (this == &other)
        return (*this);
    BaseBlock::operator=(other);
    locations = other.locations;
    listen = other.listen;
    server_names = other.server_names;
    return (*this);
}

Server::~Server()
{
}

void Server::set_locations(std::vector<Location>& locations)
{
    this->locations.insert(this->locations.end(), locations.begin(), locations.end());
}
void Server::add_location(Location& location)
{
    this->locations.push_back(location);
}
void Server::remove_location(Location& location)
{
    (void)location; // Placeholder for future implementation
}

void Server::set_server_names(std::set<std::string>& names)
{
    this->server_names.insert(names.begin(), names.end());
}
void Server::add_server_name(std::string& name)
{
    this->server_names.insert(name);
}
void Server::remove_server_name(std::string& name)
{
    this->server_names.erase(name);
}

void Server::set_listen(std::vector<std::pair<std::string, int> >& listen)
{
    this->listen = listen;
}

void Server::add_listen(std::pair<std::string, int>& listen)
{
    this->listen.push_back(listen);
}

void Server::remove_listen(std::pair<std::string, int>& listen)
{
    (void)listen; // Placeholder for future implementation
}

const std::vector<Location> Server::get_locations() const
{
    return (this->locations);
}

const std::vector<std::pair<std::string, int> > Server::get_listen() const
{
    return (this->listen);
}

const std::set<std::string> Server::get_server_names() const
{
    return (this->server_names);
}
