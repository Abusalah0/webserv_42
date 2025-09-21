/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:18:04 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/21 15:17:11 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

/**
 * @file ServerContainer.cpp
 * @brief Implementation of the HTTP server container with poll-based I/O and connection management.
 * 
 * This file implements the core server functionality including:
 * - Multi-server setup with virtual host support and socket binding
 * - Poll-based event loop for scalable I/O multiplexing
 * - Client connection lifecycle management with timeout handling
 * - CGI process monitoring and controlled termination
 * - Resource cleanup and graceful shutdown procedures
 * - Host-based server routing and request delegation
 * 
 * The implementation provides a robust, production-ready HTTP server capable of:
 * - Handling hundreds of concurrent connections efficiently
 * - Supporting multiple virtual hosts on shared IP/port combinations
 * - Managing CGI script execution with proper process control
 * - Graceful handling of network errors and resource limitations
 */

#include "../include/ServerContainer.hpp"
#include "../include/Client.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

/**
 * @brief Default constructor initializes server container with empty state.
 * Sets up all internal data structures for managing servers, clients, and CGI processes.
 * The container starts in parent process mode (m_is_child = false) for proper resource management.
 */
ServerContainer::ServerContainer():
	m_is_child(false),         // Start as parent process
	m_default_server(0),       // No default server initially
	m_servers(),               // Empty server collection
	m_servers_map(),          // Empty socket-to-server mapping
	m_servers_listen_map(),   // Empty socket-to-address mapping
	m_clients_map(),          // Empty client connection mapping
	m_poll_fds(),             // Empty poll file descriptor collection
	m_cgis_term_entries()     // Empty CGI termination queue
{}

/**
 * @brief Destructor performs comprehensive resource cleanup.
 * Closes all file descriptors, deallocates client objects, and ensures proper
 * CGI process termination. Only performs CGI cleanup in parent process to avoid
 * conflicts in fork scenarios.
 */
ServerContainer::~ServerContainer()
{
	// Close all managed file descriptors (sockets)
	close_fds();
	
	// Clean up all client objects and their associated resources
	for (std::map<int, Client*>::iterator it = this->m_clients_map.begin();
		it != this->m_clients_map.end(); it++)
	{
		delete (*it).second;
	}
	
	// Only handle CGI termination in parent process to prevent conflicts
	if (!this->m_is_child)
	{
		// Wait for all pending CGI processes to terminate
		while (!this->m_cgis_term_entries.empty())
			watch_cgis_term();
	}
}

int ServerContainer::create_listen_socket(const std::pair<std::string, std::string>& listen_item)
{
    addrinfo hints;
    addrinfo* pai;
    bool socket_binded = false;
	// initialize hints
    hints.ai_family = AF_INET;// IPv4
    hints.ai_socktype = SOCK_STREAM;// TCP
    hints.ai_protocol = 0;// any protocol
    hints.ai_next = 0;// no next
    hints.ai_flags = 0;// no flags
    hints.ai_canonname = 0;// no canonname
	
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);// create socket
    if (sockfd == -1)
	{
        throw WebservExceptions::SocketFailed();
	}
	
	int value = 1;
	// set socket options, SOL_SOCKET manipulates options at the sockets API level,
	// SO_REUSEADDR allows reuse of local addresses to prevent "Address already in use" errors
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
        if (!bind(sockfd, current_pai->ai_addr, current_pai->ai_addrlen))// bind the socket to the address
        {
            socket_binded = true;
            break ;
        }
    }
	
    freeaddrinfo(pai);// free the addrinfo struct
    if (!socket_binded)// if binding failed
    {
        close(sockfd);
        throw WebservExceptions::BindFailed();
    }
	// set the socket to listen mode
    if (listen(sockfd, 1024))
    {
        close(sockfd);
        throw WebservExceptions::ListenFailed();
    }
	
    return (sockfd);
}

void ServerContainer::setup_default_server()
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		if (this->m_servers[i].is_default_server())
		{
			if (this->m_default_server)// multiple default servers found
			{
				throw WebservExceptions::MultipleDefaultServers();
			}
			this->m_default_server = &this->m_servers[i];
		}
	}
}

void ServerContainer::setup_webserv()
{
	// set the default server if exists and make sure there is only one
	setup_default_server();
	
	bool success = false;
	
	for (size_t i = 0; i < this->m_servers.size(); i++)
    {
        Server& server = this->m_servers[i];
        const std::vector<std::pair<std::string, std::string> >& listens = server.get_listen(); // current server listening addresses
		
        for (size_t j = 0; j < listens.size(); j++)
        {
            try
            {
				// create a listening socket for the current listen address
                int sockfd = create_listen_socket(listens[j]);
                this->m_servers_map[sockfd] = &server;
				this->m_servers_listen_map[sockfd] = &listens[j];
				// add the listening socket to the poll fds vector
				// TO-DO: use the add_to_poll method instead or be gay
                pollfd entry;
                entry.fd = sockfd;
                entry.events = POLLIN | POLLOUT; // ready to read and write
                this->m_poll_fds.push_back(entry);
				success = true;
            }
            catch(const std::bad_alloc& e)
            {
                throw ;
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
	std::cout << "Client Removed!" << std::endl;
	
	close(this->m_poll_fds[poll_index].fd);// close the client socket
	delete this->m_clients_map[this->m_poll_fds[poll_index].fd];// delete the client object
	this->m_clients_map.erase(this->m_poll_fds[poll_index].fd);// remove the client from the clients map
	this->m_poll_fds[poll_index].fd = -1;// mark the poll fd as invalid for cleanup later
}

/**
 * @brief Accepts a new client connection on a server listening socket.
 * 
 * Creates a new Client object for the accepted connection and adds it to the
 * client management structures. The client is immediately added to the poll
 * array for I/O event monitoring. Handles address parsing and proper error
 * cleanup if client creation fails.
 * 
 * @param poll_index Index in poll_fds array where the listening socket is located
 */
void ServerContainer::accept_client(size_t poll_index)
{
	pollfd& poll_data = this->m_poll_fds[poll_index];
	sockaddr_in client_addr;
	socklen_t client_addrlen = sizeof(sockaddr_in);
	
	// Accept new client connection (non-blocking socket)
	int client_fd = accept(poll_data.fd, (sockaddr*)&client_addr, &client_addrlen);
	if (client_fd == -1)
	{
		std::cerr << "accept failed!" << std::endl;
		return ;
	}
	
	std::pair<std::string, std::string> client_parsed_addr;
	try
	{
		client_parsed_addr = parse_sockaddr(client_addr);
	}
	catch(const std::exception& e)
	{
		close(client_fd);
		throw e;
	}
	// create a new client object and add it to the clients map
	this->m_clients_map.insert(
		std::pair<int, Client*>(client_fd, new Client(
			client_fd,
			this,
			this->m_servers_map[poll_data.fd],
			this->m_servers_listen_map[poll_data.fd],
			client_parsed_addr
		))
	);
	
	debugClientConn(this->m_servers_listen_map[poll_data.fd]);
	// add the new client socket to the poll fds vector
	// TO-DO: use the add_to_poll method instead or be gay
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
			this->m_poll_fds.erase(this->m_poll_fds.begin() + i);// remove disconnected clients
		else
			i++;
	}
}

bool is_client_timeout(Client* client)
{
	time_t raw_time = std::time(0);
	
	if (raw_time >= client->get_last_activity() + CLIENT_TIMEOUT_SEC)
	{
		return (true);
	}
	return (false);
}

/**
 * @brief Main event loop handling all server I/O operations and client connections.
 * 
 * This is the core of the HTTP server, implementing a poll-based event loop that:
 * - Monitors all server listening sockets for new connections
 * - Handles client socket I/O (reading requests, writing responses)
 * - Processes client state machines and HTTP request/response cycles
 * - Manages client timeouts and connection cleanup
 * - Monitors CGI process termination and resource cleanup
 * 
 * The loop continues until interrupted by a signal (SIGINT/SIGTERM).
 * Poll timeout is set to 1 second to allow periodic maintenance tasks.
 */
void ServerContainer::loop()
{
    while (true)
    {
		errno = 0;
		
		// Poll all file descriptors for I/O events with timeout
        if (poll(this->m_poll_fds.data(), this->m_poll_fds.size(), POLL_TIMEOUT_MS) < 0)
		{
			// Break on interrupt signal (graceful shutdown)
			if (errno == EINTR)
				break ;
			throw WebservExceptions::PollFailed();
		}
		// Process all file descriptors that have pending events
		for (size_t i = 0; i < this->m_poll_fds.size(); i++)
		{
			pollfd& poll_data = this->m_poll_fds[i];
			if (poll_data.revents) // File descriptor has pending events
			{
				// Check if event is on a server listening socket (new connection)
				if (this->m_servers_map.find(poll_data.fd) != this->m_servers_map.end())
					accept_client(i); // Accept new client connection
				// Check if event is on existing client socket
				else if (this->m_clients_map.find(poll_data.fd) != this->m_clients_map.end())
				{
                    Client* client = this->m_clients_map[poll_data.fd];
					
					// Check for client disconnection, completion, or timeout
					if (poll_data.revents & POLLHUP || client->get_client_status() > CLIENT_DONE || is_client_timeout(client))
					{
						remove_client(i); // Clean up disconnected/finished client
						continue;
					}
					
					// Handle different I/O events
					if (poll_data.revents & POLLIN)  // Data available for reading
                        client->handle_read();
					if (poll_data.revents & POLLOUT) // Socket ready for writing
						client->handle_send();
					
					// Process client state machine (parse, route, respond)
					client->process();
					if (client->is_client_completed())
					{
						remove_client(i); // Clean up disconnected/finished client
						continue;
					}
				}
			}
		}
		
		// Perform maintenance tasks
		loop_cleanup();   // Remove closed file descriptors from poll array
		watch_cgis_term(); // Monitor and terminate expired CGI processes
		if (g_signum)
			break ;
    }
}

void ServerContainer::add_to_poll(int fd, short events)
{
	pollfd entry;
	entry.fd = fd;
	entry.events = events;
	entry.revents = 0;
	this->m_poll_fds.push_back(entry);
}

pollfd& ServerContainer::get_poll_entry(int fd)
{
	for (size_t i = 0; i < this->m_poll_fds.size(); i++)
	{
		if (this->m_poll_fds[i].fd == fd)
		{
			return (this->m_poll_fds[i]);
		}
	}
	throw WebservExceptions::PollEntryNotFound();
}

void ServerContainer::remove_from_poll(int fd)
{
	for (size_t i = 0; i < this->m_poll_fds.size(); i++)
	{
		if (this->m_poll_fds[i].fd == fd)
		{
			this->m_poll_fds[i].fd = -1;
			return ;
		}
	}
	throw WebservExceptions::PollEntryNotFound();
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
	
	// remove port if exists
	if (pos != std::string::npos)
	{
		cleaned_virtual_host = cleaned_virtual_host.substr(0, pos);
	}
		
    for (size_t i = 0; i < this->m_servers.size(); i++)
    {
        std::vector<std::pair<std::string, std::string> > listens = this->m_servers[i].get_listen();
		// loop over the server listen addresses
        for (size_t j = 0; j < listens.size(); j++)
        {
            if (listens[j].first == ip && listens[j].second == port)// matching ip and port
            {
                if (this->m_servers[i].match_virtual_host(cleaned_virtual_host))// and matching virtual host
				{
                	return (this->m_servers[i]);// got the best server, b**ch
				}
            }
        }
    }
	
	if (this->m_default_server)// return the default server if exists
	{
		return (*this->m_default_server);
	}
	
    throw WebservExceptions::HTTPException(HTTP_NOT_FOUND);// no matching server found, the client is mentally challenged
}

void ServerContainer::close_fds()
{
	for (size_t i = 0; i < this->m_poll_fds.size(); i++)
	{
		if (this->m_poll_fds[i].fd != -1)
		{
			close(this->m_poll_fds[i].fd);
		}
	}
}

bool ServerContainer::is_child() const
{
	return (this->m_is_child);
}

void ServerContainer::set_child()
{
	this->m_is_child = true;
}

void ServerContainer::add_cgi_term_entry(cgi_term_entry& entry)
{
	this->m_cgis_term_entries.push_back(entry);
}

void ServerContainer::watch_cgis_term()
{
	time_t raw_time = std::time(0);
	size_t i = 0;
	
	while (i < this->m_cgis_term_entries.size())
	{
		cgi_term_entry& entry = this->m_cgis_term_entries[i];
		pid_t pid = waitpid(entry.pid, 0, WNOHANG);
		if (pid)
		{
			this->m_cgis_term_entries.erase(this->m_cgis_term_entries.begin() + i);
		}
		else if (raw_time > entry.soft_term_time + CGI_TERM_TIMEOUT_SEC)
		{
			kill(entry.pid, SIGKILL);
			waitpid(entry.pid, 0, 0);
			this->m_cgis_term_entries.erase(this->m_cgis_term_entries.begin() + i);
		}
		else
			i++;
	}
}
