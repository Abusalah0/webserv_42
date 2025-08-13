/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:28:59 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/13 14:29:55 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOCATION_HPP
# define LOCATION_HPP  

#include "BaseBlock.hpp"
#include <set>
#include <string>
#include <map>
#include <vector>
#include <utility>

class Location : public BaseBlock
{
    bool dir_listing;
    std::set<std::string> allowed_methods;
    std::string upload_path;
    std::map<std::string, std::string> cgi_handlers;
};


#endif