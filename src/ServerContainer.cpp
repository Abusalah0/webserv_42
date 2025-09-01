/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/01 22:56:48 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ServerContainer.hpp"
#include "Client.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

ServerContainer::ServerContainer() : m_default_server(0), m_servers() {}

ServerContainer::ServerContainer(const ServerContainer& other) :
    m_default_server(other.m_default_server), m_servers(other.m_servers)
{}

ServerContainer& ServerContainer::operator=(const ServerContainer& other)
{
    if (this != &other)
    {
        m_servers = other.m_servers;
        m_default_server = other.m_default_server;
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
	for (std::map<int, Client*>::iterator it = this->m_clients_map.begin();
		it != this->m_clients_map.end(); it++)
	{
		delete (*it).second;
	}
}

int ServerContainer::create_listen_socket(const std::pair<std::string, std::string>& listen_item)
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
	{
        throw WebservExceptions::SocketFailed();
	}
	int value = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)))
	{
		close(sockfd);
		throw WebservExceptions::SocketFailed();
	}
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

void ServerContainer::setup_default_server()
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		if (this->m_servers[i].is_default_server())
		{
			if (this->m_default_server)
				throw WebservExceptions::MultipleDefaultServers();
			this->m_default_server = &this->m_servers[i];
		}
	}
}

void ServerContainer::setup_webserv()
{
	setup_default_server();
	bool success = false;
	for (size_t i = 0; i < this->m_servers.size(); i++)
    {
        Server& server = this->m_servers[i];
        const std::vector<std::pair<std::string, std::string> >& listens = server.get_listen();
        for (size_t i = 0; i < listens.size(); i++)
        {
            try
            {
                int sockfd = create_listen_socket(listens[i]);
                this->m_servers_map[sockfd] = &server;
				this->m_servers_listen_map[sockfd] = &listens[i];
                pollfd entry;
                entry.fd = sockfd;
                entry.events = POLLIN | POLLOUT;
                this->m_poll_fds.push_back(entry);
				success = true;
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
	if (!success)
		throw WebservExceptions::SetupFailed();
}

static void debugClientConn(const std::pair<std::string, std::string>* server_addr)
{
	std::cout << "Client connected to " << server_addr->first << ":" << server_addr->second << std::endl;
}

void ServerContainer::remove_client(size_t poll_index)
{
	std::cout << "Client disconnected!" << std::endl;
	close(this->m_poll_fds[poll_index].fd);
	delete this->m_clients_map[this->m_poll_fds[poll_index].fd];
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
		std::pair<int, Client*>(client_fd, new Client(client_fd, this, this->m_servers_map[poll_data.fd], this->m_servers_listen_map[poll_data.fd]))
	);
	debugClientConn(this->m_servers_listen_map[poll_data.fd]);
	pollfd entry;
	entry.fd = client_fd;
	entry.events = POLLIN | POLLOUT;
	entry.revents = 0;
	this->m_poll_fds.push_back(entry);
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
		errno = 0;
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
                    Client* client = this->m_clients_map[poll_data.fd];
					if (poll_data.revents & POLLHUP || client->get_client_status() > CLIENT_DONE)
					{
						remove_client(i);
						continue;
					}
					if (poll_data.revents & POLLIN)
                        client->handle_read();
					if (poll_data.revents & POLLOUT)
						client->handle_send();
					client->process();
				}
			}
		}
		loop_cleanup();
		if (g_signum == SIGINT)
			break;
    }
}

void ServerContainer::add_to_poll(int fd)
{
	pollfd entry;
	entry.fd = fd;
	entry.events = 0;
	entry.revents = 0;
	this->m_poll_fds.push_back(entry);
}

void ServerContainer::remove_from_poll(int fd)
{
	for (size_t i = 0; i < this->m_poll_fds.size(); i++)
	{
		if (this->m_poll_fds[i].fd == fd)
		{
			this->m_poll_fds[i].fd = -1;
			return;
		}
	}
}

void ServerContainer::add_server(const Server& server)
{
    this->m_servers.push_back(server);
	
}

const std::vector<Server>& ServerContainer::get_servers() const
{
    return (m_servers);
}

const Server& ServerContainer::get_best_server(const std::string& ip, const std::string& port, const std::string& virtual_host) const
{
	std::string cleaned_virtual_host = virtual_host;
	size_t pos = virtual_host.find(":");
	if (pos != std::string::npos)
		cleaned_virtual_host = cleaned_virtual_host.substr(0, pos);
    for (size_t i = 0; i < this->m_servers.size(); i++)
    {
        std::vector<std::pair<std::string, std::string> > listens = this->m_servers[i].get_listen();
        for (size_t j = 0; j < listens.size(); j++)
        {
            if (listens[j].first == ip && listens[j].second == port)
            {
                if (this->m_servers[i].match_virtual_host(cleaned_virtual_host))
                	return (this->m_servers[i]);
            }
        }
    }
	if (this->m_default_server)
		return *this->m_default_server;
    throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);
}
