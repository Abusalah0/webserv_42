/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_baseblock.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 00:55:32 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 10:59:14 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

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
        // std::cout << "location current directive -- >" << tokens[pos].word << std::endl;
        if (is_brace_open(tokens[pos]))
            throw_parse_error("Unexpected brace inside simple directive");
        if (is_brace_close(tokens[pos]))
            return ;
        ++pos; // skip argument token
    }
}

void skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    // pos expected to be at the location path token when called by skip_server_block
    expect_token(tokens, pos);
    // if (!is_word(tokens[pos]))
        // throw_parse_error("Expected location path after 'location'");
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
            // std::cout << "finished location skiping" << std::endl;
            break;
        }
        // should be a directive word
        // if (!is_word(tokens[pos]))
        //     throw_parse_error("Expected directive inside location");
        skip_directive(tokens, pos);
    }
}

void skip_server_block(const std::vector<t_token> &tokens, std::size_t &pos)
{
    // pos expected to be at '{' when called (or at token right after "server")
    expect_token(tokens, pos);
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after server");
    ++pos; // enter server block

    while (true)
    {
        expect_token(tokens, pos);
        // std::cout << "server current directive -- >" << tokens[pos].word << std::endl;
        if (is_brace_close(tokens[pos]))
        {
            ++pos; // consume '}'
            // std::cout << "finished server parsing" << std::endl;
            break;
        }
        
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected directive inside server");
       
        if (tokens[pos].word == "location")
        {
            ++pos; // skip the 'location' token
            skip_location_block(tokens, pos);
            // ++pos;
            continue;
        }
        // otherwise skip a general directive (name + args + ;)
        skip_directive(tokens, pos);
    }
}

void store_directive(BaseBlock &baseBlock, const std::string &directive, const std::vector<t_token> &tokens, std::size_t &pos)
{
    // store_directive expects pos at the directive token (e.g. "root")
    expect_token(tokens, pos);
    if (!is_word(tokens[pos]))
        throw_parse_error("Expected directive word");
    // sanity check
    // if (tokens[pos].word != directive)
    // {
        // not fatal, but indicates mismatch — adjust pos to directive
        // we will still proceed, but this shouldn't normally happen
    // }

    ++pos; // skip directive word
    expect_token(tokens, pos);

    if (directive == "client_max_body_size")
    {
        if (!(is_word(tokens[pos]) || is_number(tokens[pos])))
            throw_parse_error("Expected size after client_max_body_size");
        baseBlock.set_client_max_body_size(tokens[pos].word);
        ++pos;
        expect_token(tokens, pos);
        if (!is_semicolon(tokens[pos]))
            throw_parse_error("Expected ';' after client_max_body_size");
        ++pos; // consume ;
    }
    else if (directive == "error_page")
    {
        // accept one-or-more numeric codes then a path then ;
        std::set<std::string> codes;
        while (is_number(tokens[pos]))
        {
            codes.insert(tokens[pos].word.c_str());
            ++pos;
            expect_token(tokens, pos);
        }
        
        if (codes.empty())
            throw_parse_error("Expected at least one error code for error_page");
        
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected page path after error_page code(s)");

        std::string page_path = tokens[pos].word;
        ++pos;
        expect_token(tokens, pos);
        if (!is_semicolon(tokens[pos]))
            throw_parse_error("Expected ';' after error_page");
        ++pos; // consume ';'

        for (std::size_t i = 0; i < codes.size(); ++i)
            baseBlock.insert_error_page(codes, page_path);
    }
    else if (directive == "redirect")
    {
        // similar to error_page (codes + path)
        std::set<std::string> codes;
        while (is_number(tokens[pos]))
        {
            codes.insert(tokens[pos].word.c_str());
            ++pos;
            expect_token(tokens, pos);
        }
        
        if (codes.empty())
            throw_parse_error("Expected at least one redirect code");

        if (!is_word(tokens[pos]))
            throw_parse_error("Expected redirect path after code(s)");

        std::string path = tokens[pos].word;
        ++pos;
        expect_token(tokens, pos);
        
        if (!is_semicolon(tokens[pos]))
            throw_parse_error("Expected ';' after redirect");
        ++pos;
        for (std::size_t i = 0; i < codes.size(); ++i)
            baseBlock.insert_redirect_page(codes, path);
    }
    else if (directive == "root")
    {
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected root path after 'root'");
        baseBlock.set_root(tokens[pos].word);
        ++pos;
        expect_token(tokens, pos);
        if (!is_semicolon(tokens[pos]))
            throw_parse_error("Expected ';' after root");
        ++pos;
    }
    else if (directive == "auto_index" || directive == "autoindex")
    {
        if (!is_word(tokens[pos]))
            throw_parse_error("Expected 'on' or 'off' after 'auto_index'");
        baseBlock.set_auto_index(tokens[pos].word);
        ++pos;
        expect_token(tokens, pos);
        if (!is_semicolon(tokens[pos]))
            throw_parse_error("Expected ';' after auto_index");
        ++pos;
    }
    else if (directive == "index")
    {
        std::vector<std::string> index_pages;
        // collect until semicolon
        while (true)
        {
            expect_token(tokens, pos);
            if (is_semicolon(tokens[pos]))
            {
                ++pos;
                break;
            }
            if (!is_word(tokens[pos]))
                throw_parse_error("Expected index filename or ';'");
            index_pages.push_back(tokens[pos].word);
            ++pos;
        }

        if (index_pages.empty())
            throw_parse_error("index: expected at least one filename");

        baseBlock.insert_index_pages(index_pages);
    }
    else
    {
        throw_parse_error("Unknown directive: " + directive);
    }
}

void parse_baseblock(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t pos)
{
    // note: pos passed by value intentionally (two-pass design).
    expect_token(tokens, pos);

    while (pos < tokens.size())
    {
        expect_token(tokens, pos);
        // std::cout << "http current directive -- >" << tokens[pos].word << std::endl;
        
        // if we hit the closing brace of http block, we're done
        if (is_brace_close(tokens[pos]))
        {
            // std::cout << "finished http parsing" << std::endl;
            return ;
        }

        // if token is 'server', skip the entire server block (we only want http defaults here)
        if (is_word(tokens[pos]) && tokens[pos].word == "server")
        {
            ++pos; // skip 'server' word
            // now pos should point to '{'
            skip_server_block(tokens, pos);
            // continue scanning for other http directives
            // std::cout << "EXACT directive after server skip -- >" << tokens[pos].word << std::endl;
            continue;
        }

        // if token is an http-level directive store it in baseBlock
        if (is_http_directive(tokens[pos]))
        {
            std::string directive = tokens[pos].word;
            // store_directive expects pos at directive token and will advance pos
            store_directive(baseBlock, directive, tokens, pos);
            continue;
        }

        // std::cout << "error http current directive -- >" << tokens[pos].word << std::endl;
        // any other token at http level is an error
        throw_parse_error("Unexpected token at http level: " + tokens[pos].word);
    }
    if (is_brace_close(tokens[pos]))
    {
        // std::cout << "finished http parsing" << std::endl;
        return ;
    }
    // if we exit loop without hitting a '}', the config is malformed
    throw_parse_error("Unexpected end of tokens while parsing http defaults");
}
