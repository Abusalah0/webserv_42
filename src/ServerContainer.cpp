/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 14:43:38 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContainer.hpp"
#include <stdexcept>

ServerContainer::ServerContainer() : servers(), default_exists(false) {}

ServerContainer::ServerContainer(const ServerContainer& other) :
    servers(other.servers), default_exists(other.default_exists)
{}

ServerContainer& ServerContainer::operator=(const ServerContainer& other)
{
    if (this != &other)
    {
        servers = other.servers;
        default_exists = other.default_exists;
    }
    return (*this);
}

ServerContainer::ServerContainer(const std::vector<Server>& servers) 
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        add_server(servers[i]);
    }
    
}

ServerContainer::~ServerContainer() {}

void ServerContainer::add_server(const Server& server)
{
    if (server.get_default() && default_exists)    
    {
        return; //throw some shit
    }
    default_exists = default_exists & server.get_default();
    servers.push_back(server);
}

void ServerContainer::remove_server(int index)
{
    if (index < 0 || index >= static_cast<int>(servers.size()))
    {
        throw std::out_of_range("Index out of range");
    }
    servers.erase(servers.begin() + index);
    // server_map.erase(index);
}

// Server* ServerContainer::get_server(int index)
// {
//     if (server_map.find(index) != server_map.end())
//     {
//         return server_map[index];
//     }
//     return (NULL);
// }

const std::vector<Server>& ServerContainer::get_servers() const
{
    return (servers);
}
