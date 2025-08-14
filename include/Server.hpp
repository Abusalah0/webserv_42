/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:26:51 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/14 09:27:15 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "BaseBlock.hpp"
# include <vector>
# include <set>
# include <string>
# include <utility>
# include "Location.hpp"

class Server : public BaseBlock
{
    private:
        std::vector<Location> locations;
        std::vector<std::pair<std::string, int> > listen;
        std::set<std::string> server_names;
    public:
        Server();
        Server(const Server& other);
        Server(const BaseBlock& baseBlock,
               const std::vector<Location>& locations,
               const std::vector<std::pair<std::string, int> >& listen,
               const std::set<std::string>& serverNames);
        Server& operator=(const Server& other);
        ~Server();
        
};

#endif // SERVER_HPP
