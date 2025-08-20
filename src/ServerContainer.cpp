/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 17:19:13 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContainer.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

ServerContainer::ServerContainer() : default_exists(false), servers() {}

ServerContainer::ServerContainer(const ServerContainer& other) :
    default_exists(other.default_exists), servers(other.servers)
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

int ServerContainer::create_listen_socket(std::pair<std::string, std::string> listen_item)
{
    addrinfo hints;
    addrinfo* pai;
    bool socket_binded = false;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_next = 0;
    hints.ai_flags = 0;
    hints.ai_canonname = 0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        throw WebservExceptions::SocketFailed();
    if (getaddrinfo(listen_item.first.c_str(), listen_item.second.c_str(), &hints, &pai))
    {
        close(sockfd);
        throw WebservExceptions::GAIFailed();
    }
    for (addrinfo* current_pai = pai;
        current_pai != NULL;
        current_pai = current_pai->ai_next)
    {
        if (!bind(sockfd, current_pai->ai_addr, current_pai->ai_addrlen))
        {
            socket_binded = true;
            break;
        }
    }
    freeaddrinfo(pai);
    if (!socket_binded)
    {
        close(sockfd);
        throw WebservExceptions::BindFailed();
    }
    if (listen(sockfd, 1024))
    {
        close(sockfd);
        throw WebservExceptions::ListenFailed();
    }
    return sockfd;
}

void ServerContainer::setup_webserv()
{
    for (std::vector<Server>::iterator it = this->servers.begin();
        it != this->servers.end();
        it++)
    {
        Server* server = &(*it);
        std::vector<std::pair<std::string, std::string> > listens = server->get_listen();
        for (size_t i = 0; i < listens.size(); i++)
        {
            try
            {
                int sockfd = create_listen_socket(listens[i]);
                this->servers_map[sockfd] = server;
                pollfd entry;
                entry.fd = sockfd;
                entry.events = POLLIN | POLLOUT;
                this->poll_fds.push_back(entry);
            }
            catch(const std::bad_alloc& e)
            {
                throw;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

void ServerContainer::loop()
{
    while (true)
    {
        poll(this->poll_fds.data(), this->poll_fds.size(), -1);
        std::cout << "LOOP COMPLETE!" << std::endl;
    }
}

Server* ServerContainer::get_sock_server(int sockfd)
{
    return this->servers_map[sockfd];
}

void ServerContainer::add_server(const Server& server)
{
    if (server.get_default() && default_exists)
    {
        throw WebservExceptions::ADefaultServerAlreadyExists();
    }
    default_exists = default_exists & server.get_default();
    servers.push_back(server);
}

//lol
//void ServerContainer::remove_server(int index)
//{
//    if (index < 0 || index >= static_cast<int>(servers.size()))
//    {
//        throw std::out_of_range("Index out of range");
//    }
//    servers.erase(servers.begin() + index);
//}

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
