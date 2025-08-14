/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 09:26:18 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContainer.hpp"
#include <stdexcept>

ServerContainer::ServerContainer() {}

ServerContainer::ServerContainer(const ServerContainer& other) :
    servers(other.servers)
    // server_map(other.server_map)
{}

ServerContainer& ServerContainer::operator=(const ServerContainer& other)
{
    if (this != &other)
    {
        servers = other.servers;
        // server_map = other.server_map;
    }
    return (*this);
}

ServerContainer::ServerContainer(const std::vector<Server>& servers) :
    servers(servers)
    // server_map(server_map)
{}

ServerContainer::~ServerContainer() {}

void ServerContainer::add_server(const Server& server)
{
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
