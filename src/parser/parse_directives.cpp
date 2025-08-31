/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_directives.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 12:36:13 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/31 03:24:22 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "parser.hpp"

void    parse_root_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    // next token must be a word (the path)
    if (!is_word(tokens[pos]))
    {
        throw_parse_error("Expected root path after 'root'");
    }
    baseBlock.set_root(tokens[pos].word);// set root in baseblock
}

void skip_directive(const std::vector<t_token> &tokens, std::size_t &pos)
{
    // pos currently points at the directive word or at the first token of the directive
    expect_token(tokens, pos);
    // Skip tokens until a semicolon is found (or throw on malformed config)
    while (true)
    {
        expect_token(tokens, pos);
        if (is_semicolon(tokens[pos]))
        {
            ++pos; // consume ';'
            return ;
        }
        // If we find a brace here, treat it as an error for simple directives
        if (is_brace_open(tokens[pos]))
            throw_parse_error("Unexpected brace inside simple directive");
        if (is_brace_close(tokens[pos]))
            return ;
        ++pos; // skip argument token
    }
}

void    skip_over_semicolon(const std::vector<t_token> &tokens, std::size_t &pos)
{
    expect_token(tokens, pos);
    
    if (!is_semicolon(tokens[pos]))
    {
        throw_parse_error("Expected ';' after directive");
    }
    ++pos; // skip ';'
}

void    parse_redirect_directive(std::vector<t_token> const &tokens, BaseBlock &baseblock, std::size_t &pos)
{
    std::set<std::string> codes;
    // collect codes until we hit a non-number (the path)
    while (is_number(tokens[pos]))
    {
        codes.insert(tokens[pos].word.c_str());
        ++pos;
        expect_token(tokens, pos);
    }
    // must have at least one code
    if (codes.empty())
    {
        throw_parse_error("Expected at least one redirect code");
    }
    // next token must be the path
    if (!is_word(tokens[pos]))
    {
        throw_parse_error("Expected redirect path after code(s)");
    }

    std::string path = tokens[pos].word;
    // insert each code with the same path
    for (std::size_t i = 0; i < codes.size(); ++i)
    {
        baseblock.insert_redirect_page(codes, path);
    }
}

void    parse_error_page_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    std::set<std::string> codes;
    // collect codes until we hit a non-number (the page)   
    while (is_number(tokens[pos]))
    {
        codes.insert(tokens[pos].word.c_str());
        ++pos;
        expect_token(tokens, pos);
    }
    // must have at least one code
    if (codes.empty())
    {
        throw_parse_error("Expected at least one error code for error_page");
    }
    // next token must be the page path
    if (!is_word(tokens[pos]))
    {
        throw_parse_error("Expected page path after error_page code(s)");
    }

    std::string page_path = tokens[pos].word;
    // insert each code with the same page path
    for (std::size_t i = 0; i < codes.size(); ++i)
    {
        baseBlock.insert_error_page(codes, page_path);
    }
}

void    parse_index_direcitive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    std::vector<std::string> index_pages;
    // collect until semicolon
    while (is_word(tokens[pos]))
    {
        expect_token(tokens, pos);
        
        index_pages.push_back(tokens[pos].word);
        ++pos;
    }
    // must have at least one index page
    if (index_pages.empty())
    {
        throw_parse_error("index: expected at least one filename");
    }
    // next token must be a semicolon
    if (!is_semicolon(tokens[pos]))
    {
        throw_parse_error("Expected ';' after index directive");
    }
    --pos; // step back to let skip_over_semicolon handle the semicolon

    baseBlock.insert_index_pages(index_pages);// insert into baseblock
}

void    parse_auto_index_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    if (!is_word(tokens[pos])) // must be 'on' or 'off'
    {
        throw_parse_error("Expected 'on' or 'off' after 'auto_index'");
    }
    baseBlock.set_auto_index(tokens[pos].word);// set auto_index in baseblock
}


void parse_client_max_body_size_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    if (!(is_word(tokens[pos]) || is_number(tokens[pos]))) // must be a number possibly followed by a unit
    {
        throw_parse_error("Expected size after client_max_body_size");
    }
    baseBlock.set_client_max_body_size(tokens[pos].word);// set size in baseblock
}

__attribute__((unused)) static int check_port_number(std::string &port)
{
    // check port length
    if (port.empty() || port.length() > 5)
    {
        throw_parse_error("Invalid port number, port too large ;)");
    }
    // make sure there is only digits
    for (size_t i = 0; i < port.length(); i++)
    {
        if (!isdigit(port[i]))
            throw_parse_error("Expected digits for the port number");
    }
    
    int pnum = strtol(port.c_str(), NULL, 10);
    if (pnum > 65535)// check range
        throw_parse_error("Invalid port number, max port number is 65535");
    
    return (pnum);
}

__attribute__((unused)) static std::string& check_listen_address(std::string &address)
{
    for (size_t i = 0; i < address.length(); i++)
    {
        if (!(isdigit(address[i]) || address[i] == '.'))
            throw_parse_error("Invalid character in listen address");
    }
    //check each octet from the address
    size_t start = 0;
    size_t end = address.find('.');
    while (end != std::string::npos)
    {
        // extract octet
        std::string octet = address.substr(start, end - start);
        if (octet.empty() || octet.length() > 3)
            throw_parse_error("Invalid IP address in listen directive");
        // check range
        int octet_num = strtol(octet.c_str(), NULL, 10);
        if (octet_num < 0 || octet_num > 255)
            throw_parse_error("IP address octet out of range (0-255)");
        // move to next octet
        start = end + 1;
        end = address.find('.', start);
    }
    return (address);
}

void    parse_listen_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos)
{
    std::pair< std::string, std::string> listen;
        
    // look for colon in host:port format
    size_t colon = tokens[pos].word.find_first_of(':');
    if (colon == std::string::npos)// throw exception if no colon found
    {
        throw_parse_error("Expected host:port format after listen directive");
    }
    // extract address part
    std::string address = tokens[pos].word.substr(0, colon);
    if (address.empty())// throw exception if address is empty
    {
        throw_parse_error("Expected valid address before ':' in listen directive");
    }
    // check_listen_address(address);// validate address part
    listen.first = address;// store address part
    
    std::string port_str = tokens[pos].word.substr(colon + 1);// 
    if (port_str.empty())// throw exception if port part is empty
    {
        throw_parse_error("Expected valid port number after ':' in listen directive");
    }
    // check_port_number(port_str);// validate port part
    listen.second = port_str;// store port part
    
    srv.add_listen(listen);// add to server object
}

void parse_server_name_directive(const std::vector<t_token> &tokens, Server &srv, std::size_t &pos)
{
    // must have at least one name
    if (!is_word(tokens[pos]))
    {
        throw_parse_error("Expected a Name after the server_name directive");
    }
    
    std::set<std::string> names;
    
    while(is_word(tokens[pos]))// collect all names
    {
        names.insert(tokens[pos].word);// add name to set
        ++pos;
        expect_token(tokens, pos);
    }
    srv.set_server_names(names);// set names in server object
    --pos; // to counter the extra ++pos at the end of the function
}

void parse_limit_directive(const std::vector<t_token> &tokens, Location &loc, std::size_t &pos)
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