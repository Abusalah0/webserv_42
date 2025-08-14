/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:15:57 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 09:14:20 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : BaseBlock(), 
                   locations(),
                   listen(),
                   server_names() {}

Server::Server(const Server& other) : 
        BaseBlock(other),
        locations(other.locations),
        listen(other.listen),
        server_names(other.server_names)
{}

Server::Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, int> >& listen,
               const std::set<std::string>& serverNames)
    : BaseBlock(baseBlock),
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
    // Destructor logic if needed
}

