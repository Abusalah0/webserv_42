/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:42:52 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 17:17:47 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTAINER_HPP
# define SERVERCONTAINER_HPP

# include "Client.hpp"
# include "Exceptions.hpp"
# include <vector>
# include <map>
#include <poll.h>

class ServerContainer
{
    private:
        bool m_default_exists; // true if a server with the is_default attribute set as true exists in the servers vector
        std::vector<Server> m_servers;
        std::map<int, Server*> m_servers_map;
		std::map<int, sockaddr_in> m_servers_addr_map;
		std::map<int, Client> m_clients_map;
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
        int create_listen_socket(std::pair<std::string, std::string> listen_item, sockaddr_in& server_addr);
        void setup_webserv();
        void add_server(const Server& server);
        void loop();
		void accept_client(size_t poll_index);
		void remove_client(size_t poll_index);
		void loop_cleanup();
        Server* get_sock_server(int sockfd);
        //void remove_server(int index);
        //Server* get_server(int index);
        const std::vector<Server>& get_servers() const;
};

#endif // SERVERCONTAINER_HPP
