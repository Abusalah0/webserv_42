/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/26 13:12:04 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContainer.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

ServerContainer::ServerContainer() : m_default_exists(false), m_servers() {}

ServerContainer::ServerContainer(const ServerContainer& other) :
    m_default_exists(other.m_default_exists), m_servers(other.m_servers)
{}

ServerContainer& ServerContainer::operator=(const ServerContainer& other)
{
    if (this != &other)
    {
        m_servers = other.m_servers;
        m_default_exists = other.m_default_exists;
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

ServerContainer::~ServerContainer()
{
	for (size_t i = 0; i < this->m_poll_fds.size(); i++)
		close(this->m_poll_fds[i].fd);
}

int ServerContainer::create_listen_socket(std::pair<std::string, std::string> listen_item, sockaddr_in& server_addr)
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
			server_addr = *(sockaddr_in*)current_pai->ai_addr;
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
    for (std::vector<Server>::iterator it = this->m_servers.begin();
        it != this->m_servers.end();
        it++)
    {
        Server* server = &(*it);
        std::vector<std::pair<std::string, std::string> > listens = server->get_listen();
		sockaddr_in server_addr;
        for (size_t i = 0; i < listens.size(); i++)
        {
            try
            {
                int sockfd = create_listen_socket(listens[i], server_addr);
                this->m_servers_map[sockfd] = server;
				this->m_servers_addr_map[sockfd] = server_addr;
                pollfd entry;
                entry.fd = sockfd;
                entry.events = POLLIN | POLLOUT;
                this->m_poll_fds.push_back(entry);
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

static void debugClientConn(sockaddr_in& client_addr, sockaddr_in& server_addr)
{
	char client_addr_arr[16];
	char server_addr_arr[16];
	inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_addr_arr, 16);
	inet_ntop(AF_INET, &server_addr.sin_addr.s_addr, server_addr_arr, 16);
	std::cout << "Client " << client_addr_arr << ":" << ntohs(client_addr.sin_port)
		<< " connected to " << server_addr_arr << ":" << ntohs(server_addr.sin_port) << std::endl;
}

void ServerContainer::remove_client(size_t poll_index)
{
	std::cout << "Client disconnected!" << std::endl;
	close(this->m_poll_fds[poll_index].fd);
	this->m_clients_map.erase(this->m_poll_fds[poll_index].fd);
	this->m_poll_fds[poll_index].fd = -1;
}

void ServerContainer::accept_client(size_t poll_index)
{
	pollfd& poll_data = this->m_poll_fds[poll_index];
	sockaddr_in client_addr;
	socklen_t client_addrlen = sizeof(sockaddr_in);
	int client_fd = accept(poll_data.fd, (sockaddr*)&client_addr, &client_addrlen);
	if (client_fd == -1)
	{
		std::cerr << "accept failed!" << std::endl;
		return;
	}
	this->m_clients_map.insert(
		std::pair<int, Client>(client_fd, Client(client_fd, *this->m_servers_map[poll_data.fd], client_addr))
	);
	pollfd entry;
	entry.fd = client_fd;
	entry.events = POLLIN | POLLOUT;
	this->m_poll_fds.push_back(entry);
	debugClientConn(client_addr, this->m_servers_addr_map[poll_data.fd]);
}

void ServerContainer::loop_cleanup()
{
	size_t i = 0;
	while (i < this->m_poll_fds.size())
	{
		if (this->m_poll_fds[i].fd == -1)
			this->m_poll_fds.erase(this->m_poll_fds.begin() + i);
		else
			i++;
	}
}

void ServerContainer::loop()
{
    while (true)
    {
        if (poll(this->m_poll_fds.data(), this->m_poll_fds.size(), -1) < 0)
		{
			if (errno == EINTR)
				break;
			throw WebservExceptions::PollFailed();
		}
		for (size_t i = 0; i < this->m_poll_fds.size(); i++)
		{
			pollfd& poll_data = this->m_poll_fds[i];
			if (poll_data.revents)
			{
				if (this->m_servers_map.find(poll_data.fd) != this->m_servers_map.end())
					accept_client(i);
				else if (this->m_clients_map.find(poll_data.fd) != this->m_clients_map.end())
				{
                    Client& client = this->m_clients_map[poll_data.fd];
					if (poll_data.revents & POLLHUP)
					{
						remove_client(i);
						continue;
					}
					if (poll_data.revents & POLLIN)
					{
                        client.handle_read();
					}
				}
			}
		}
		if (g_signum == SIGINT)
			break;
		loop_cleanup();
    }
}

Server* ServerContainer::get_sock_server(int sockfd)
{
    return this->m_servers_map[sockfd];
}

void ServerContainer::add_server(const Server& server)
{
    this->m_servers.push_back(server);
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
    return (m_servers);
}
