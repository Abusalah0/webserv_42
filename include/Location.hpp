/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:28:59 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/09 17:11:05 by abdsalah         ###   ########.fr       */
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
        std::set<std::string> m_allowed_methods;
        std::string m_upload_path;
        std::string m_cgi_extension;
		std::string m_cgi_pass;

    public:
        Location();
        Location(const Location& other);
        Location(const BaseBlock& base_block);
        Location(const BaseBlock& base_block,
                const std::set<std::string>& methods,
                const std::string& upload_path,
                const std::string& cgi_extension,
				const std::string& cgi_pass);


        Location& operator=(const Location& other);
        ~Location();

        void set_allowed_methods(const std::set<std::string>& methods);
        const std::set<std::string>& get_allowed_methods() const;
        void set_upload_path(const std::string& path);
        const std::string& get_upload_path() const;
        void set_cgi_extension(const std::string& extension);
        const std::string& get_cgi_extension() const;
		void set_cgi_pass(const std::string& cgi_pass);
        const std::string& get_cgi_pass() const;
		bool is_cgi_requirments(const std::string& target) const;
        void add_allowed_method(const std::string& method);
        void remove_allowed_method(const std::string& method);
		bool is_method_allowed(const std::string& method) const;
};

#endif // LOCATION_HPP
