/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 00:20:29 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/26 14:33:45 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

void skip_server_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    // pos expected to be at '{' when called.
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after server");
    ++pos; // enter server block

    while (true)
    {
        expect_token(tokens, pos);
        
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // consume '}'
            return ;
        }
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside server");
        if (tokens[pos].word == "location")
        {
            ++pos; // skip the 'location' token
            skip_location_block(tokens, pos);
            continue ;
        }
        // otherwise skip a general directive (name + args + ;)
        skip_directive(tokens, pos);
    }
}

static void store_server_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos)
{
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected directive word");
        
    std::string directive = tokens[pos].word;
    
    ++pos;
    expect_token(tokens, pos);
    
    if (directive == "listen")
       parse_listen_directive(tokens, srv, pos);
    else if (directive == "root")
       parse_root_directive(tokens, srv, pos);
    else if (directive == "autoindex" || directive == "auto_index")
        parse_auto_index_directive(tokens, srv, pos);
    else if (directive == "server_name")
       parse_server_name_directive(tokens, srv, pos);
    else if (directive == "error_page")
        parse_error_page_directive(tokens, srv, pos);
    else if (directive == "location")
    {
        skip_location_block(tokens, pos);
        return ;        
    }
    else if (directive == "redirect")
        parse_redirect_directive(tokens, srv, pos);
    else if (directive == "index")
        parse_index_direcitive(tokens, srv, pos);
    else
        throw_parse_error("Uknown directive inside server block");
        
    ++pos;// skip over the last token of the directive
    skip_over_semicolon(tokens, pos);
}

static Server process_server_block(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    Server srv(baseBlock);
    
    ++pos; // skip {
    std::size_t start_pos = pos;// first token inside server block
    while (true)// store the directives inside the server block, but skip location blocks for now
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; break ; // skip '}' and exit
        }
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside server block");
        // process the directive
        store_server_directive(tokens, srv, pos);
    }
    // now we store location inside the server block, with 
    while (start_pos < pos)
    {
        expect_token(tokens, start_pos);
        
        if (is_brace_close(tokens[start_pos]))
            break ;
        if (tokens[start_pos].word == "location")// found a location block
        {
            ++start_pos; // skip the 'location' token
            Location loc = parse_location_block(tokens, start_pos, srv);
            srv.add_location(loc);
            continue ;
        }
        // otherwise skip a general directive (name + args + ;)
        skip_directive(tokens, start_pos);
    }
    return (srv);
}

void parse_servers(const std::vector<t_token> &tokens, ServerContainer &server_container, BaseBlock &baseBlock, size_t pos)
{
    std::string tok;

    if (tokens.empty())
        throw std::runtime_error("Empty token list");
    // parse http block directives
    while (true)
    {
        expect_token(tokens, pos);
        // check end of http block
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // consume '}'
            return ;
        }
        // check for server directive
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
