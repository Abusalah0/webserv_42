/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:26:51 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/22 01:43:25 by abdsalah         ###   ########.fr       */
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
        std::vector<Location> locations;
        std::vector<std::pair<std::string, int> > listen;
        std::set<std::string> server_names;
        bool                    is_default;
    public:
        // construtors
        Server();
        Server(BaseBlock& baseBlock);
        Server(const Server& other);
        Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, int> >& listen,
               const std::set<std::string>& serverNames,
               bool is_default);
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
        void set_listen(std::vector<std::pair<std::string, int> >& listen);
        void add_listen(std::pair<std::string, int>& listen);
        void remove_listen(std::pair<std::string, int>& listen);

        // is default methodes
        void set_default(bool& is_default);
        bool get_default() const;

        // getters
        const std::vector<Location> get_locations() const;
        const std::vector<std::pair<std::string, int> > get_listen() const;
        const std::set<std::string> get_server_names() const;

};

#endif // SERVER_HPP
