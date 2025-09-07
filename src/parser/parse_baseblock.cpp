/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_baseblock.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 00:55:32 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/07 22:20:20 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../include/parser.hpp"

static void store_directive(BaseBlock &baseBlock, const std::string &directive, const std::vector<t_token> &tokens, std::size_t &pos)
{
    expect_token(tokens, pos);

    if (!is_word(tokens[pos]))
        throw_parse_error("Expected directive word");

    ++pos; // skip directive word
    expect_token(tokens, pos);

	if (directive == "client_max_body_size")
        parse_client_max_body_size_directive(tokens, baseBlock, pos);
    else if (directive == "error_page")
        parse_error_page_directive(tokens, baseBlock, pos);
    else if (directive == "root")
        parse_root_directive(tokens, baseBlock, pos);
    else if (directive == "autoindex")
        parse_auto_index_directive(tokens, baseBlock, pos);
    else if (directive == "index")
        parse_index_direcitive(tokens, baseBlock, pos);
    else
        throw_parse_error("Unknown directive: " + directive);

    ++pos; // move past the last directive argument
    skip_over_semicolon(tokens, pos);
}

void parse_baseblock(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t pos)
{
    // note: pos passed by value intentionally (two-pass design).
    expect_token(tokens, pos);

    while (pos < tokens.size())
    {
        expect_token(tokens, pos);
        // if we hit the closing brace of http block, we're done
        if (is_brace_close(tokens[pos]))
            // finished parsing http block
            return ;
        // if token is 'server', skip the entire server block (we only want http defaults here)
        if (is_word(tokens[pos]) && tokens[pos].word == "server")
        {
            ++pos; // skip 'server' word
            // now pos should point to '{'
            skip_server_block(tokens, pos);
            // continue scanning for other http directives
            continue ;
        }
        // if token is an http-level directive store it in baseBlock
        if (is_http_directive(tokens[pos]))
        {
            std::string directive = tokens[pos].word;
            // store_directive expects pos at directive token and will advance pos
            store_directive(baseBlock, directive, tokens, pos);
            // continue scanning for other http directives
            continue ;
        }
        // any other token at http level is an error
        throw_parse_error("Unexpected token at http level: " + tokens[pos].word);
    }
    if (is_brace_close(tokens[pos]))
        // finished parsing http block
        return ;
    // if we exit loop without hitting a '}', the config is malformed
    // in case pos is out of bounds, we also throw an error
    throw_parse_error("Unexpected end of tokens while parsing http defaults");
}
