/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:42:52 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/18 22:20:44 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTAINER_HPP
# define SERVERCONTAINER_HPP

//# include "Client.hpp"
# include "../include/Server.hpp"
# include "Exceptions.hpp"
# include <vector>
# include <map>
# include <poll.h>
# include <sys/wait.h>

# define CLIENT_TIMEOUT_SEC 10 // client inactivity timeout in seconds
# define CGI_TERM_TIMEOUT_SEC 2 // time to wait after soft termination before force killing the CGI process
# define POLL_TIMEOUT_MS 1000 // poll timeout in milliseconds

/**
 * @brief Structure representing a CGI termination entry.
 * It contains the soft termination time and the process ID of the CGI process.
 * This is used to monitor and kill CGI processes that have exceeded their soft termination time.
 * @note The soft termination time is the time after which the CGI process should be
 * terminated if it has not already exited.
 * @note The process ID is used to identify and kill the CGI process.
 * @see ServerContainer::watch_cgis_term()
 */
struct cgi_term_entry
{
	time_t soft_term_time;
	pid_t pid;
};

class Client;

class ServerContainer
{
    private:
		bool m_is_child;// used to silent exceptions and prevent killing conflicts
        Server* m_default_server;// pointer to the default server if exists
        std::vector<Server> m_servers;// vector of server objects
        std::map<int, Server*> m_servers_map;// map of listening socket fds to server objects
		std::map<int, const std::pair<std::string, std::string>* > m_servers_listen_map;// map of listening socket fds to their corresponding listen address (ip, port)
		std::map<int, Client*> m_clients_map;// map of client socket fds to client objects
        std::vector<pollfd> m_poll_fds;// vector of pollfd structures for polling sockets
		std::deque<cgi_term_entry> m_cgis_term_entries;// deque of cgi termination entries to monitor and kill if needed
    public:
        // constructors and destructor
        ServerContainer();
        ~ServerContainer();
        
        /**
         * @brief Creates a listening socket for the given listen address (IP and port).
         * If the socket creation or binding fails, an exception is thrown.
         * @param listen_item A pair representing the listen address (IP and port).
         * @throws SocketFailed exception if socket creation fails.
         * @throws BindFailed exception if binding the socket fails.
         * @throws ListenFailed exception if setting the socket to listen fails.
         * @return The file descriptor of the created listening socket.
         */
        int create_listen_socket(const std::pair<std::string, std::string>& listen_item);
        
        /**
         * @brief Sets up the web server by creating listening sockets for each server's listen addresses and adding them to the poll fds vector.
         * If no listening sockets can be created, an exception is thrown.
         * @throws SetupFailed exception if no listening sockets can be created.
         * @return void
         */
        void setup_webserv();
        
        /**
         * @brief Sets up the default server by checking the servers vector for a server marked as default.
         * If multiple default servers are found, an exception is thrown.
         * If no default server is found, the default server pointer remains NULL.
         * @throws MultipleDefaultServers exception if more than one default server is found.
         * @return void
         */
		void setup_default_server();
        
        /**
         * @brief Adds a server to the container.
         * @param server The server object to add.
         * @return void
         */
        void add_server(const Server& server);
        
        /**
         * @brief Accepts a new client connection on the listening socket at the given poll index.
         * A new Client object is created for the accepted connection and added to the clients map and
         * poll fds vector. If the accept call fails, an exception is thrown.
         * @param poll_index The index of the poll fds vector where the listening socket is
         * located.
         * @throws AcceptFailed exception if the accept call fails.
         * @return void
         */
		void accept_client(size_t poll_index);

        /**
         * @brief Removes a client from the container by closing its socket, deleting its Client object,
         * and removing its entry from the clients map and poll fds vector.
         * @param poll_index The index of the poll fds vector where the client's socket is located.
         * @return void
         */
		void remove_client(size_t poll_index);
        
        /**
         * @brief The main loop of the server container that handles incoming connections and client requests.
         * It uses the poll system call to monitor the listening sockets and client sockets for events.
         * If a listening socket is ready, it accepts a new client connection.
         * If a client socket is ready, it processes the client's request.
         * If a client is inactive for too long or has completed its request, it is removed.
         * The loop continues until a termination signal is received.
         * @return void
         */
        void loop();
        
        /**
         * @brief Cleans up the poll fds vector by removing entries with fd set to -1.
         * This is used to remove closed sockets from the poll fds vector.
         * @return void
         */
		void loop_cleanup();

        // Server* get_sock_server(int sockfd);
        
        // iterators
        std::vector<Server>::iterator begin() { return m_servers.begin(); }
        std::vector<Server>::const_iterator end() { return m_servers.end(); }

        /**
         * @brief Finds and returns the best matching server for the given IP, port, and virtual host.
         * The best matching server is determined by matching the IP and port with the server's listen
         * addresses, and then matching the virtual host with the server's server names.
         * If no matching server is found, the default server is returned else an exception is thrown.
         * @param ip The IP address of the incoming request.
         * @param port The port of the incoming request.
         * @param virtual_host The virtual host (server name) of the incoming request.
         * @return A reference to the best matching Server object.
         * @note If multiple servers match the IP and port, the first one found is used.
         * @throws NoDefaultServer exception if no default server is set and no matching server is found.
         */
        const Server& get_best_server(const std::string& ip, const std::string& port, const std::string& virtual_host) const;
        
        /**
         * @brief Gets the vector of servers in the container.
         * @return A constant reference to the vector of Server objects.
         */
        const std::vector<Server>& get_servers() const;

        /**
         * @brief Adds a file descriptor to the poll fds vector with the specified events.
         * If the fd already exists in the poll fds vector, its events are updated.
         * @param fd The file descriptor to add.
         * @param events The events to monitor for the file descriptor.
         * @return void
         */
        void add_to_poll(int fd, short events = 0);
        
        /**
         * @brief Gets the pollfd entry for the given file descriptor.
         * If the fd is not found in the poll fds vector, an exception is thrown.
         * @param fd The file descriptor to find.
         * @throws PollEntryNotFound exception if the fd is not found in the poll fds vector.
         * @return A reference to the pollfd entry for the given fd.
         */
		pollfd& get_poll_entry(int fd);

        /**
         * @brief Removes a file descriptor from the poll fds vector.
         * If the fd is not found in the poll fds vector, an exception is thrown.
         * @param fd The file descriptor to remove.
         * @throws PollEntryNotFound exception if the fd is not found in the poll fds vector.
         * @return void
         */
        void remove_from_poll(int fd);

        /**
         * @brief Closes all file descriptors in the poll fds vector and clears the vector.
         * This is used to clean up resources when the server container is destroyed.
         * @return void
         */
		void close_fds();

        /**
         * @brief Checks if the server container is running in a child process.
         * This is used to prevent certain operations (like killing processes) in child processes.
         * @return true if the container is in a child process, false otherwise.
         * @note The container is marked as a child process by calling set_child().
         */
		bool is_child() const;

        /**
         * @brief Marks the server container as running in a child process.
         * This is used to prevent certain operations (like killing processes) in child processes.
         * @return void
         */
		void set_child();

        /**
         * @brief Adds a CGI termination entry to the deque of CGI termination entries.
         * This is used to monitor and kill CGI processes that have exceeded their soft termination time.   
         * @param entry The CGI termination entry to add.
         * @return void
         */
		void add_cgi_term_entry(cgi_term_entry& entry);

        /**
         * @brief Monitors the deque of CGI termination entries and kills any CGI processes that have exceeded their soft termination time.
         * This is called periodically in the main loop to ensure that CGI processes do not run indefinitely.
         * @return void
         */
		void watch_cgis_term();
};

#endif // SERVERCONTAINER_HPP
