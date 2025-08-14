/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:42:52 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 09:25:28 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTAINER_HPP
# define SERVERCONTAINER_HPP

# include "Server.hpp"
# include <vector>
# include <map>

class ServerContainer
{
    private:
        std::vector<Server> servers;
        // std::map<int, Server*> server_map;
    public:
        ServerContainer();
        ServerContainer(const ServerContainer& other);
        ServerContainer(const std::vector<Server>& servers);
        ServerContainer& operator=(const ServerContainer& other);
        ~ServerContainer();

        void add_server(const Server& server);
        void remove_server(int index);
        Server* get_server(int index);
        const std::vector<Server>& get_servers() const;
};

#endif // SERVERCONTAINER_HPP
