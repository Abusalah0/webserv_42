/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 00:20:29 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/22 01:52:56 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

__attribute__ ((unused)) static Location parse_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
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
        if (is_brace_close(tokens[pos])) {
            ++pos; // skip }           
            break;
        }
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside location block");
        // process the directive
    }

    return (loc);
}

static Server process_server_block(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after server");
    ++pos; // skip {

    Server srv(baseBlock);
    
    while (true)
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // skip }
            break;
        }
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside server block");
        // process the directive
    }
    return (srv);
}

__attribute__((unused)) void parse_servers( const std::vector<t_token> &tokens, ServerContainer &serverContainer, BaseBlock &baseBlock, size_t pos)
{
    std::string tok;

    if (tokens.empty())
        throw std::runtime_error("Empty token list");

    ++pos;
    expect_token(tokens, pos);
    
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after http");
   
    ++pos; // skip {

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
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive at http level");
        
        tok = tokens[pos].word;
        ++pos;

        if (tok == "server")
        {
            expect_token(tokens, pos);
            if (!is_brace_open(tokens[pos]))
                throw_parse_error("Expected '{' after server");
            
            // parse_server_block expects pos at the brace_open
            Server srv = process_server_block(tokens, baseBlock, pos);
            serverContainer.add_server(srv);
        }
        else
        {
            throw_parse_error("Unknown directive at http level: " + tok);
        }
    }
}
