/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_location.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 12:06:03 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 12:40:40 by abdsalah         ###   ########.fr       */
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
        parse_root_directive(tokens, loc, pos);
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
        if (methods.empty())
            throw_parse_error("Expected at least one method after limit");
        loc.set_allowed_methods(methods);
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
    
    // std::cout << "token after parsing location directive : " << tokens[pos].word << std::endl;
    ++pos;
    expect_token(tokens, pos);
    if (!is_semicolon(tokens[pos]))
    {
        throw_parse_error("Expected semicolon at the end of the directive");
    }
    // std::cout << "Found ; after location directive" << std::endl;
    // ++pos;
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
            break;
        }
        
        // process the directive
        
        store_location_directive(tokens, pos, loc);
        ++pos;
    }
    return (loc);
}
