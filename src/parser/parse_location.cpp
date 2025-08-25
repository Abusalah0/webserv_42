/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_location.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 12:06:03 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 17:27:24 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

void skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    // pos expected to be at the location path token when called by skip_server_block
    expect_token(tokens, pos);
    
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected location path after 'location'");
    ++pos; // skip the location path

    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after location");
    ++pos; // enter location block
    // inside location, skip directives until matching '}'
    while (true)
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // consume '}'
            return ;
        }
        // should be a directive word
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside location");
        skip_directive(tokens, pos);
    }
}

void    store_location_directive(const std::vector<t_token> &tokens, std::size_t &pos, Location &loc)
{
    if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside location block"); 
            
    std::string directive = tokens[pos].word;
    
    ++pos;// skip directive word
    expect_token(tokens, pos);
    
    if (directive == "root")
        parse_root_directive(tokens, loc, pos);
    else if (directive == "limit")
        parse_limit_directive(tokens, loc, pos);
    else if (directive == "cgi")
    {
        if(is_word(tokens[pos]))
            loc.set_cgi_handlers(tokens[pos].word);
    }
    else
        throw_parse_error("Unknown directive inside location block");
    
    ++pos;
    expect_token(tokens, pos);

    if (!is_semicolon(tokens[pos]))
        throw_parse_error("Expected semicolon at the end of the directive");
}

Location parse_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    expect_token(tokens, pos);
    // next token must be a WORD (the location path)
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected location path after 'location'");

    Location loc;

    std::string location_path = tokens[pos].word;
    ++pos;
    loc.set_upload_path(location_path); // using upload_path to store location path temporarily
    // now expect opening brace
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after location path");
        
    ++pos; // skip {
    // parse directives until }
    while (true)
    {
        expect_token(tokens, pos);
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // skip }           
            break ;
        }
        // process the directive
        store_location_directive(tokens, pos, loc);
        ++pos;
    }
    return (loc);
}
