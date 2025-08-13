/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContainer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 15:42:52 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/13 15:44:20 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTAINER_HPP
#define SERVERCONTAINER_HPP
#include "Server.hpp"
#include <vector>
#include <map>

class ServerContainer
{
    
    private:
        std::vector<Server> servers;
        std::map<int, Server*> server_map;
};

#endif // SERVERCONTAINER_HPP