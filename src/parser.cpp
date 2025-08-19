/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 22:00:55 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/20 00:50:50 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenizer.hpp"
#include "BaseBlock.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerContainer.hpp"
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cstddef>

// i will throw better errors later.
static void throw_parse_error(const std::string &msg, std::size_t pos)
{
    throw std::runtime_error("Parse error at token index " + std::to_string(pos) + ": " + msg);
}

static bool is_word(const t_token &t)  
{
    return (t.type == WORD);
}
static bool is_number(const t_token &t)
{
    return (t.type == NUMBER);
}
static bool is_semicolon(const t_token &t)
{
    return (t.type == SEMICOLON);
}
static bool is_brace_open(const t_token &t)
{
    return (t.type == BRACE_OPEN);
}
static bool is_brace_close(const t_token &t)
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
static void expect_token(const std::vector<t_token> &tokens, std::size_t pos)
{
    if (pos >= tokens.size())
        throw std::runtime_error("Unexpected end of tokens");
}

static Location parse_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    expect_token(tokens, pos);
    // next token must be a WORD (the location path)
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected location path after 'location'", pos);

    std::string location_path = tokens[pos].word;
    ++pos;

    // now expect opening brace
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after location path", pos);
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
            throw_parse_error("Expected directive inside location block", pos);
        // process the directive
    }

    return (loc);
}

void    parse_baseblock(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t pos)
{
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
            throw_parse_error("Expected directive at http level", pos);
        
        tok = tokens[pos].word;
        ++pos;

        if (tok == )
        {
           
        }
        else
        {
            throw_parse_error("Unknown directive at http level: " + tok, pos);
        }
    }
}

static Server process_server_block(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after server", pos);
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
            throw_parse_error("Expected directive inside server block", pos);
        // process the directive
    }
    return (srv);
}

void parse_servers( const std::vector<t_token> &tokens, ServerContainer &serverContainer, BaseBlock &baseBlock, size_t pos)
{
    std::string tok;

    if (tokens.empty())
        throw std::runtime_error("Empty token list");

    expect_token(tokens, pos);

    if (!is_word(tokens[pos]) || tokens[pos].word != "http")
        throw_parse_error("Expected 'http' at start", pos);

    ++pos;
    expect_token(tokens, pos);
    
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after http", pos);
   
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
            throw_parse_error("Expected directive at http level", pos);
        
        tok = tokens[pos].word;
        ++pos;

        if (tok == "server")
        {
            expect_token(tokens, pos);
            if (!is_brace_open(tokens[pos]))
                throw_parse_error("Expected '{' after server", pos);
            
            // parse_server_block expects pos at the brace_open
            Server srv = process_server_block(tokens, baseBlock, pos);
            serverContainer.add_server(srv);
        }
        else
        {
            throw_parse_error("Unknown directive at http level: " + tok, pos);
        }
    }
}

void parser(const std::vector<t_token> &tokens, ServerContainer &serverContainer)
{
    std::size_t pos = 0;
    std::string tok;
    BaseBlock baseBlock;
    
    if (tokens.empty())
        throw std::runtime_error("Empty token list");

    expect_token(tokens, pos);

    if (!is_word(tokens[pos]) || tokens[pos].word != "http")
        throw_parse_error("Expected 'http' at start", pos);

    ++pos;
    expect_token(tokens, pos);
    
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after http", pos);
    // skip the opening brace
    ++pos;
    // parse http block directives
    parse_baseblock(tokens, baseBlock, pos);
    // parse server blocks
    parse_servers(tokens, serverContainer, baseBlock, pos);

    
}
