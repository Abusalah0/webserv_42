/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/30 21:35:17 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContainer.hpp"
#include <stdexcept>

ServerContainer::ServerContainer() :
            default_exists(false),
            servers()
{}

ServerContainer::ServerContainer(const ServerContainer& other) :
        default_exists(other.default_exists),
        servers(other.servers)
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
    servers.push_back(server);
}

//lol
void ServerContainer::remove_server(int index)
{
    if (index < 0 || index >= static_cast<int>(servers.size()))
    {
        throw std::out_of_range("Index out of range");
    }
    servers.erase(servers.begin() + index);
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

const Server& ServerContainer::get_best_server(const std::string& ip, const std::string& port, const std::string& virtual_host) const
{
    
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        std::vector<std::pair<std::string, std::string> > listens = this->servers[i].get_listen();
        for (size_t j = 0; j < listens.size(); j++)
        {
            if (listens[j].first == ip && listens[j].second == port)
            {
                try
                {
                    this->servers[i].match_virtual_host(virtual_host);
                    return (this->servers[i]);
                }
                catch (const std::exception& e)
                {
                    // Continue searching
                }
            }
        }
    }
    throw std::runtime_error("No matching server found");
}
