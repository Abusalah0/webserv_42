/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:28:59 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/19 23:31:29 by abdsalah         ###   ########.fr       */
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

/**
 * @brief Represents a location block in the server configuration.
 * Location blocks define specific handling rules for URL paths, including
 * allowed HTTP methods, file upload paths, and CGI configuration.
 */
class Location : public BaseBlock
{
    private:
        std::set<std::string> m_allowed_methods; ///< Set of allowed HTTP methods for this location
        std::string m_upload_path; ///< Path for file uploads (also used as location route)
        std::string m_cgi_extension; ///< File extension that triggers CGI execution
		std::string m_cgi_pass; ///< Path to the CGI interpreter executable

    public:
        // Constructors and Destructor
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

        // HTTP Methods Management
        /**
         * @brief Sets the allowed HTTP methods for this location.
         * @param methods Set of HTTP method names (e.g., "GET", "POST", "DELETE")
         */
        void set_allowed_methods(const std::set<std::string>& methods);
        
        /**
         * @brief Gets the set of allowed HTTP methods for this location.
         * @return Constant reference to the set of allowed HTTP methods
         */
        const std::set<std::string>& get_allowed_methods() const;
        
        /**
         * @brief Adds a single HTTP method to the allowed methods set.
         * @param method HTTP method name to add (e.g., "GET", "POST")
         */
        void add_allowed_method(const std::string& method);
        
        /**
         * @brief Removes a single HTTP method from the allowed methods set.
         * @param method HTTP method name to remove
         */
        void remove_allowed_method(const std::string& method);
        
        /**
         * @brief Checks if a specific HTTP method is allowed for this location.
         * @param method HTTP method name to check
         * @return true if the method is allowed, false otherwise
         */
		bool is_method_allowed(const std::string& method) const;

        // Upload Path Management
        /**
         * @brief Sets the upload path for file uploads (also serves as location route).
         * @param path File system path for uploads
         */
        void set_upload_path(const std::string& path);
        
        /**
         * @brief Gets the upload path for this location.
         * @return Constant reference to the upload path string
         */
        const std::string& get_upload_path() const;

        // CGI Configuration
        /**
         * @brief Sets the file extension that triggers CGI execution.
         * @param extension File extension (e.g., ".py", ".php")
         */
        void set_cgi_extension(const std::string& extension);
        
        /**
         * @brief Gets the CGI file extension for this location.
         * @return Constant reference to the CGI extension string
         */
        const std::string& get_cgi_extension() const;
        
        /**
         * @brief Sets the path to the CGI interpreter executable.
         * @param cgi_pass Path to CGI interpreter (e.g., "/usr/bin/python3")
         */
		void set_cgi_pass(const std::string& cgi_pass);
        
        /**
         * @brief Gets the CGI interpreter path for this location.
         * @return Constant reference to the CGI pass string
         */
        const std::string& get_cgi_pass() const;
        
        /**
         * @brief Checks if the target file meets CGI execution requirements.
         * @param target Target file path to check
         * @return true if the target matches CGI extension requirements, false otherwise
         */
		bool is_cgi_requirments(const std::string& target) const;
};

#endif // LOCATION_HPP
