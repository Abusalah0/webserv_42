/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:11:56 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 11:04:08 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"

// i will throw better errors later.
void throw_parse_error(const std::string &msg)
{
    
    throw std::runtime_error("Parse error: " + msg);
}

bool is_word(const t_token &t)  
{
    return (t.type == WORD);
}
bool is_number(const t_token &t)
{
    return (t.type == NUMBER);
}
bool is_semicolon(const t_token &t)
{
    return (t.type == SEMICOLON);
}
bool is_brace_open(const t_token &t)
{
    return (t.type == BRACE_OPEN);
}
bool is_brace_close(const t_token &t)
{
    return (t.type == BRACE_CLOSE);
}

/**
 * @brief Ensure the token at the given position exists.
 * Throws an exception if the position is out of bounds.
 * @param tokens The vector of tokens to check.
 * @param pos The position to check.
 * @throws std::runtime_error if pos is out of bounds.
 */
void expect_token(const std::vector<t_token> &tokens, std::size_t pos)
{
    if (pos >= tokens.size())
        throw std::runtime_error("Unexpected end of tokens");
}

bool is_http_directive(const t_token &t)
{
    if (!is_word(t))
        return (false);

    const std::string &word = t.word;

    return (word == "error_page"
        || word == "autoindex"
        || word == "listen"
        || word == "root"
        || word == "index"
        || word == "client_max_body_size"
    );
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
        
        // Print server-specific properties
        std::cout << "\n--- SERVER PROPERTIES ---\n";
        
        // Print listen addresses and ports
        std::vector<std::pair<std::string, int> > listen_list = srv.get_listen();
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
        
        // Print server names
        std::set<std::string> server_names = srv.get_server_names();
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
        
        // Print locations
        std::vector<Location> locations = srv.get_locations();
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
                
                // Location-specific properties
                std::cout << "    Upload Path: " << loc.get_upload_path() << "\n";
                std::cout << "    CGI Handlers: " << loc.get_cgi_handlers() << "\n";
                
                std::set<std::string> methods = loc.get_allowed_methods();
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
        }
        
        std::cout << "\n";
    }
    
    std::cout << "=========================================\n";
    std::cout << "END OF SERVER CONTAINER DEBUG INFO\n";
    std::cout << "=========================================\n\n";
}
