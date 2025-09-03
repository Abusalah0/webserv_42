/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_print.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:23:35 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/03 16:21:16 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.hpp"

static void print_server_names(const std::set<std::string> &server_names)
{
    std::cout << "Server Names (" << server_names.size() << "):\n";
    if (server_names.empty())
    {
        std::cout << "  (No server names configured)\n";
    }
    else
    {
        std::size_t name_count = 1;
        for (std::set<std::string>::const_iterator it = server_names.begin(); 
             it != server_names.end(); ++it)
        {
            std::cout << "  " << name_count++ << ". " << *it << "\n";
        }
    }
}

static void print_listen(const std::vector<std::pair<std::string, std::string> > &listen_list)
{
    std::cout << "Listen Addresses (" << listen_list.size() << "):\n";
    if (listen_list.empty())
    {
        std::cout << "  (No listen addresses configured)\n";
    }
    else
    {
        for (std::size_t j = 0; j < listen_list.size(); ++j)
        {
            std::cout << "  " << (j + 1) << ". " << listen_list[j].first 
                        << ":" << listen_list[j].second << "\n";
        }
    }
  
}

static void print_error_pages(const std::map<ushort, std::string> &error_pages)
{
    std::cout << "Error Pages (" << error_pages.size() << "):\n";
    if (error_pages.empty())
    {
        std::cout << "  (No error pages configured)\n";
    }
    else
    {
        for (std::map<ushort, std::string>::const_iterator it = error_pages.begin();
                it != error_pages.end(); ++it)
        {
            std::cout << "  " << it->first << " -> " << it->second << "\n";
        }
    }

}

static void print_redirects(const std::map<ushort, std::string> &redirects)
{
    std::cout << "Redirect Pages (" << redirects.size() << "):\n";
    if (redirects.empty())
    {
        std::cout << "  (No redirect pages configured)\n";
        return;
    }
    for (std::map<ushort, std::string>::const_iterator it = redirects.begin();
         it != redirects.end(); ++it)
    {
        std::cout << "  " << it->first << " -> " << it->second << "\n";
    }
}

static void print_index_pages(const std::vector<std::string> &index_pages)
{
    std::cout << "Index Pages (" << index_pages.size() << "):\n";
    if (index_pages.empty())
    {
        std::cout << "  (No index pages configured)\n";
    }
    else
    {
        for (std::size_t idx = 0; idx < index_pages.size(); ++idx)
        {
            std::cout << "  " << (idx + 1) << ". " << index_pages[idx] << "\n";
        }
    }

}

static void print_allowed_methods(const std::set<std::string> &methods)
{
    std::cout << "    Allowed Methods (" << methods.size() << "):\n";
    if (methods.empty())
    {
        std::cout << "      (No methods configured)\n";
    }
    else
    {
        std::cout << "      ";
        for (std::set<std::string>::const_iterator method_it = methods.begin(); 
                method_it != methods.end(); ++method_it)
        {
            if (method_it != methods.begin())
                std::cout << ", ";
            std::cout << *method_it;
        }
        std::cout << "\n";
    }
}

static void print_locations(const std::vector<Location> &locations)
{
    std::cout << "\n--- LOCATIONS (" << locations.size() << ") ---\n";
    if (locations.empty())
    {
        std::cout << "  (No locations configured)\n";
    }
    else
    {
        for (std::size_t loc_idx = 0; loc_idx < locations.size(); ++loc_idx)
        {
            std::cout << "\n  Location " << (loc_idx + 1) << ":\n";
            const Location& loc = locations[loc_idx];
            
            // Location BaseBlock properties
            std::cout << "    Root: " << loc.get_root() << "\n";
            std::cout << "    Auto Index: " << (loc.get_auto_index() ? "on" : "off") << "\n";
            std::cout << "    Client Max Body Size: " << loc.get_client_max_body_size() << " bytes\n";
            
            // Location index pages
            print_index_pages(loc.get_index_pages());            
            // Location redirect pages
            print_redirects(loc.get_redirect_pages());            
            // Location error pages
            print_error_pages(loc.get_error_pages());
            // Location-specific properties
            std::cout << "    Upload Path: " << loc.get_root() << "\n";
            std::cout << "    CGI Handlers: " << loc.get_cgi_handlers() << "\n";
            
            // Allowed methods
            print_allowed_methods(loc.get_allowed_methods());            
        }
    }
    
}

void print_server_container(const ServerContainer &serverContainer)
{
    std::cout << "\n=== SERVER CONTAINER DEBUG INFO ===\n";
    
    const std::vector<Server>& servers = serverContainer.get_servers();
    std::cout << "Total servers: " << servers.size() << "\n\n";
    
    for (std::size_t i = 0; i < servers.size(); ++i)
    {
        std::cout << "##########################################\n";
        std::cout << "SERVER " << (i + 1) << ":\n";
        std::cout << "##########################################\n";
        
        const Server& srv = servers[i];
        // Print BaseBlock properties
        std::cout << "\n--- BASE BLOCK PROPERTIES ---\n";
        std::cout << "Root: " << srv.get_root() << "\n";
        std::cout << "Auto Index: " << (srv.get_auto_index() ? "on" : "off") << "\n";
        std::cout << "Client Max Body Size: " << srv.get_client_max_body_size() << " bytes\n";
        // Print index pages
        print_index_pages(srv.get_index_pages());    
        // Print redirect pages
        print_redirects(srv.get_redirect_pages());
        // Print error pages
        print_error_pages(srv.get_error_pages());        
        // Print server-specific properties
        std::cout << "\n--- SERVER PROPERTIES ---\n";
        // Print listen addresses and ports
        print_listen(srv.get_listen());
        // Print server names
        print_server_names(srv.get_server_names());        
        // Print locations
        std::vector<Location> locations = srv.get_locations();
        print_locations(locations);
        std::cout << "\n";
    }
    
    std::cout << "=========================================\n";
    std::cout << "END OF SERVER CONTAINER DEBUG INFO\n";
    std::cout << "=========================================\n\n";
}
