/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:28:59 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/29 18:17:02 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP  

# include "BaseBlock.hpp"
# include <set>
# include <string>
# include <map>
# include <vector>
# include <utility>

class Location : public BaseBlock
{
    private:
        std::set<std::string> allowed_methods;
        // std::string upload_path;
        std::string cgi_handlers;

    public:
        Location();
        Location(const Location& other);
        Location(const BaseBlock& baseBlock);
        Location(const BaseBlock& baseBlock,
                const std::set<std::string>& methods,
                // const std::string& uploadPath,
                const std::string& cgiHandlers);


        Location& operator=(const Location& other);
        ~Location();

        void set_allowed_methods(const std::set<std::string>& methods);
        const std::set<std::string>& get_allowed_methods() const;
        // void set_upload_path(const std::string& path);
        // const std::string& get_upload_path() const;
        void set_cgi_handlers(const std::string& handlers);
        const std::string& get_cgi_handlers() const;

        void add_allowed_method(const std::string& method);
        void remove_allowed_method(const std::string& method);
};

#endif // LOCATION_HPP
