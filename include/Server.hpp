/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:26:51 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/17 21:47:55 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <set>
# include <string>
# include <utility>
# include <vector>
# include "tokenizer.hpp"
# include "BaseBlock.hpp"
# include "Location.hpp"

class Server : public BaseBlock
{
    private:
		bool m_is_default; // indicates if this server is the default server
		bool m_root_location_exist; // indicates if a root location (i.e., a location with the path "/") exists
		size_t m_root_location_index; // the index of the root location in the m_locations vector, if it exists
        std::vector<Location> m_locations; // vector of Location objects representing the server's location objects
        std::vector<std::pair<std::string, std::string> > m_listen; // vector of pairs representing the server's listening addresses (IP and port)
        std::set<std::string> m_server_names; // set of server names (i.e., domain names) associated with the server
    public:
        // constructors
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
        /**
         * @brief Sets the locations for the server by inserting them into the internal Locations vector.
         * @param locations A vector of Location objects to set.
         * @return void
         */
        void set_locations(std::vector<Location>& locations);
        /**
         * @brief Adds a Location object to the server's internal Locations vector, and updates the root location if necessary.
         * @param location A Location object to add.
         * @return void
         */
        void add_location(Location& location);
        // void remove_location(Location& location);
        // Location& get_location_by_path(const std::string& path, const std::string &virtual_host) const;
        
        // server name methods
        /**
         * @brief Sets the server names for the server by inserting them into the internal server names set.
         * @param names A set of server names to set.
         * @return void
         */
        void set_server_names(std::set<std::string>& names);
        /**
         * @brief Adds a server name to the server's internal server names set.
         * @param name A server name to add.
         * @return void
         */
        void add_server_name(std::string& name);
        // void remove_server_name(std::string& name);

        // listen methods
        /**
         * @brief Sets the listen addresses for the server by copying them into the internal listen vector.
         * @param listen A vector of pairs representing the listen addresses to set.
         * @return void
         */
        void set_listen(std::vector<std::pair<std::string, std::string> >& listen);
        /**
         * @brief Adds a listen address to the server's internal listen vector.
         * @param listen A pair representing the listen address to add.
         * @return void
         */
        void add_listen(std::pair<std::string, std::string>& listen);
        // void remove_listen(std::pair<std::string, std::string>& listen);

        // is default methodes

        // getters
        const std::vector<Location> get_locations() const;
        const std::vector<std::pair<std::string, std::string> >& get_listen() const;
        const std::set<std::string> get_server_names() const;

        // utils
        /**
         * @brief Checks if the given virtual host matches any of the server's configured server names.
         * @param virtual_host The virtual host to check.
         * @return true if the virtual host matches a server name or if no server names are configured, false otherwise.
         */
        bool match_virtual_host(const std::string &virtual_host) const;
        /**
         * @brief Finds the best matching location for a given route.
         * @param route The route to match against the server's locations.
         * @return A reference to the best matching Location object.
         * @throws std::runtime_error if no matching location is found.
         */
		const Location& match_location(std::string& route) const;

		// etc
        /**
         * @brief Sets the server as the default server.
         * @return void
         */
		void set_default_server();
        /**
         * @brief Checks if the server is set as the default server.
         * @return true if the server is the default server, false otherwise.
         */
		bool is_default_server() const;
        /**
         * @brief Checks if a root location (i.e., a location with the path "/") exists.
         * @return true if a root location exists, false otherwise.
         */
		bool root_location_exist() const;
        /**
         * @brief Gets the root location (i.e., the location with the path "/").
         * @return A reference to the root Location object.
         * @throws std::runtime_error if no root location exists.
         */
		const Location& get_root_location() const;
};

/**
 * @brief Skips over a location block in the token vector.
 * @param tokens The vector of tokens to parse.
 * @param pos The current position in the token vector (will be changed).
 * @return void
 */
void skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos);

#endif // SERVER_HPP
