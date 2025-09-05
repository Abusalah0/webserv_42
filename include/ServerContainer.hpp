/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:42:52 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/05 21:47:45 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERCONTAINER_HPP
# define SERVERCONTAINER_HPP

//# include "Client.hpp"
# include "../include/Server.hpp"
# include "Exceptions.hpp"
# include <vector>
# include <map>
#include <poll.h>

#define CLIENT_TIMEOUT_SEC 10

class Client;

class ServerContainer
{
    private:
		bool m_is_child;
        Server* m_default_server;
        std::vector<Server> m_servers;
        std::map<int, Server*> m_servers_map;
		std::map<int, const std::pair<std::string, std::string>* > m_servers_listen_map;
		std::map<int, Client*> m_clients_map;
        std::vector<pollfd> m_poll_fds;
    public:
        ServerContainer();
        ServerContainer(const ServerContainer& other);
        ServerContainer(const std::vector<Server>& servers);
        ServerContainer& operator=(const ServerContainer& other);
        ~ServerContainer();
        /**
         * @brief adds a server to the vector of servers, and makes sure there is only one default server
         * @throws a ADefaultServerAlreadyExists exception when adding multiple deault servers
         * @param a server object
         */
        int create_listen_socket(const std::pair<std::string, std::string>& listen_item);
        void setup_webserv();
		void setup_default_server();
        void add_server(const Server& server);
        void loop();
		void accept_client(size_t poll_index);
		void remove_client(size_t poll_index);
		void loop_cleanup();
        Server* get_sock_server(int sockfd);
        //void remove_server(int index);
        //Server* get_server(int index);
        // typedef typename std::vector<Server>::iterator iterator;
        // typedef typename std::vector<Server>::const_iterator const_iterator;
        std::vector<Server>::iterator begin() { return m_servers.begin(); }
        std::vector<Server>::const_iterator end() { return m_servers.end(); }
        const Server& get_best_server(const std::string& ip, const std::string& port, const std::string& virtual_host) const;
        const std::vector<Server>& get_servers() const;
        void add_to_poll(int fd, short events = 0);
		pollfd& get_poll_entry(int fd);
        void remove_from_poll(int fd);
		void close_fds();
		bool is_child() const;
		void set_child();
};

#endif // SERVERCONTAINER_HPP
