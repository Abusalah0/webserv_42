/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 00:20:29 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/24 18:01:53 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

void    store_location_directive(const std::vector<t_token> &tokens, std::size_t &pos, Location &loc)
{
    if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside location block"); 
    std::string directive = tokens[pos].word;

    ++pos;
    expect_token(tokens, pos);
    
    if (directive == "root")
    {
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected root path after 'root'");
        loc.set_root(tokens[pos].word);
    }
    else if (directive == "limit")
    {
        std::set<std::string> methods;
        
        while (is_word(tokens[pos]))
        {
            methods.insert(tokens[pos].word);
            ++pos;
            expect_token(tokens, pos);
        }
        --pos;
    }
    else if (directive == "cgi")
    {
        if(is_word(tokens[pos]))
        {
            loc.set_cgi_handlers(tokens[pos].word);
        }
    }
    else
    {
        std::cout << "current directive ->" << tokens[pos].word << std::endl;
        throw_parse_error("Unknown directive inside location block");
    }
    
    std::cout << "token after parsing location directive : " << tokens[pos].word << std::endl;
    ++pos;
    expect_token(tokens, pos);
    if (!is_semicolon(tokens[pos]))
    {
        throw_parse_error("Expected semicolon at the end of the directive");
    }
    std::cout << "Found ; after location directive" << std::endl;
    // ++pos;
}

static Location parse_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    expect_token(tokens, pos);
    // next token must be a WORD (the location path)
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected location path after 'location'");

    std::string location_path = tokens[pos].word;
    ++pos;

    // now expect opening brace
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after location path");
    ++pos; // skip {

    Location loc;

    // parse directives until }
    while (true)
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // skip }           
            break;
        }
        
        // process the directive
        
        store_location_directive(tokens, pos, loc);
        ++pos;
    }
    return (loc);
}

static void store_server_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos)
{
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected directive word");
        
    std::string directive = tokens[pos].word;
    
    ++pos;
    expect_token(tokens, pos);
    
    if (directive == "listen")
    {
        std::pair< std::string, int> listen;
        
        size_t colon = tokens[pos].word.find_first_of(':');
        if (colon == std::string::npos)
            throw_parse_error("Expected host:port format after listen directive");
        
        std::string address = tokens[pos].word.substr(0, colon);
        if (address.empty())
            throw_parse_error("Expected valid address before ':' in listen directive");
        listen.first = address;
        std::string port_str = tokens[pos].word.substr(colon + 1);
        if (port_str.empty())
            throw_parse_error("Expected valid port number after ':' in listen directive");
        
        listen.second = std::strtol(port_str.c_str(), NULL, 10);
        srv.add_listen(listen);
    }
    else if (directive == "root")
    {
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected root path after 'root'");
        srv.set_root(tokens[pos].word);
        
    }
    else if (directive == "autoindex" || directive == "auto_index")
    {
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected 'on' or 'off' after 'auto_index'");
        srv.set_auto_index(tokens[pos].word);
    }
    else if (directive == "server_name")
    {
        if (!is_word(tokens[pos]))
        {
            throw_parse_error("Expected a Name after the server_name directive");
        }
        
        std::set<std::string> names;
        while(is_word(tokens[pos]))
        {
            names.insert(tokens[pos].word);
            ++pos;
            expect_token(tokens, pos);
            std::cout << "server name ->>" << tokens[pos].word << '\n';
        }
        srv.set_server_names(names);
        --pos; // to counter the extra ++pos at the end of the function
    }
    else if (directive == "error_page")
    {
        if (!is_number(tokens[pos]))
            throw_parse_error("expected a number after the error_page directive");
        std::set<std::string> codes;
        
        while (is_number(tokens[pos]))
        {
            codes.insert(tokens[pos].word);
            ++pos;
            expect_token(tokens, pos);
        }
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected error page after error codes");
        srv.insert_error_page(codes, tokens[pos].word);
    }
    else if (directive == "location")
    {
        parse_location_block(tokens, pos);
        return ;        
    }
    else
    {
        std::cout << "current directive ->" << tokens[pos].word << std::endl;   
        throw_parse_error("Uknown directive inside server block");
    }
    ++pos;
    std::cout << "token after parsing directive" << tokens[pos].word << std::endl;   
    
    // expect_token(tokens, pos);
    if (!is_semicolon(tokens[pos]))
    {
        std::cout << "why not ;?" << tokens[pos].word << std::endl;   
        throw_parse_error("Expected ';' after directive");
    }
    ++pos; // skip ';'
}

static Server process_server_block(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    Server srv(baseBlock);
    
    ++pos; // skip {
    while (true)
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // skip }
            break;
        }
        if (!is_word(tokens[pos]))
        {
            std::cout << tokens[pos].word << std::endl;
            throw_parse_error("Expected directive inside server block");
        }
        // process the directive
        std::cout << "server-->current token: " << tokens[pos].word << std::endl;
        store_server_directive(tokens, srv, pos);
    }
    return (srv);
}

void parse_servers(const std::vector<t_token> &tokens, ServerContainer &server_container, BaseBlock &baseBlock, size_t pos)
{
    std::string tok;

    if (tokens.empty())
        throw std::runtime_error("Empty token list");

    // ++pos;
    // expect_token(tokens, pos);
    
    // parse http block directives
    while (true)
    {
        expect_token(tokens, pos);
        // check end of http block
        if (is_brace_close(tokens[pos]))
        {
            ++pos;
            break;
        }
        // check for server directive
        // std::cout << "http-->server-->current token: " << tokens[pos].word << std::endl;
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive at http level");
        
            
        if (tokens[pos].word == "server")
        {
            ++pos;
            expect_token(tokens, pos);
            
            if (!is_brace_open(tokens[pos]))
                throw_parse_error("Expected '{' after server");
            
            Server srv = process_server_block(tokens, baseBlock, pos);
            server_container.add_server(srv);
        }
        else
        {
            skip_directive(tokens, pos);
        }
    }
}
