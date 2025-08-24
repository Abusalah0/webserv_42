/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:26:51 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/24 19:23:30 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <set>
# include <string>
# include <utility>
# include <vector>
# include "BaseBlock.hpp"
# include "Location.hpp"

class Server : public BaseBlock
{
    private:
        std::vector<Location> m_locations;
        std::vector<std::pair<std::string, std::string> > m_listen;
        std::set<std::string> m_server_names;
    public:
        // construtors
        Server();
        Server(BaseBlock& baseBlock);
        Server(const Server& other);
        Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, std::string> >& listen,
               const std::set<std::string>& serverNames);
        // copy operator
        Server& operator=(const Server& other);
        // destructor
        ~Server();

        // locations methods
        void set_locations(std::vector<Location>& locations);
        void add_location(Location& location);
        void remove_location(Location& location);
        
        // server name methods
        void set_server_names(std::set<std::string>& names);
        void add_server_name(std::string& name);
        void remove_server_name(std::string& name);

        // listen methods
        void set_listen(std::vector<std::pair<std::string, std::string> >& listen);
        void add_listen(const std::pair<std::string, std::string>& entry);
        void remove_listen(std::pair<std::string, int>& listen);

        // is default methodes

        // getters
        const std::vector<Location> get_locations() const;
        const std::vector<std::pair<std::string, std::string> > get_listen() const;
        const std::set<std::string> get_server_names() const;

};

#endif // SERVER_HPP
