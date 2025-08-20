/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 14:29:43 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : BaseBlock(), 
                   locations(),
                   listen(),
                   server_names(),
                   is_default(false) {}

Server::Server(const Server& other) : 
        BaseBlock(other),
        locations(other.locations),
        listen(other.listen),
        server_names(other.server_names),
        is_default(other.is_default)
{}

Server::Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, std::string> >& listen,
               const std::set<std::string>& serverNames, bool is_default)
    : BaseBlock(baseBlock),
      locations(locations),
      listen(listen),
      server_names(serverNames),
      is_default(is_default)
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
    this->locations = locations;
}
void Server::add_location(Location& location)
{
    this->locations.push_back(location);
}

//void Server::remove_location(Location& location)
//{
    
//}

void Server::set_server_names(std::set<std::string>& names)
{
    this->server_names = names;
}
void Server::add_server_name(std::string& name)
{
    this->server_names.insert(name);
}

//void Server::remove_server_name(std::string& name)
//{
//    this->server_names.erase(name);
//}

void Server::set_listen(std::vector<std::pair<std::string, std::string> >& listen)
{
    this->listen = listen;
}

void Server::add_listen(const std::string& listen)
{
	std::pair<std::string, std::string> entry;
    std::size_t pos = listen.find(':');
    entry.first = listen.substr(0, pos);
    entry.second = listen.substr(pos + 1, std::string::npos);
    this->listen.push_back(entry);
}

//void Server::remove_listen(std::pair<std::string, int>& listen)
//{
    
//}

const std::vector<Location> Server::get_locations() const
{
    return (this->locations);
}

const std::vector<std::pair<std::string, std::string> > Server::get_listen() const
{
    return (this->listen);
}

const std::set<std::string> Server::get_server_names() const
{
    return (this->server_names);
}

bool Server::get_default() const
{
    return (this->is_default);
}

void Server::set_default(bool& is_default)
{
    this->is_default = is_default;
}
