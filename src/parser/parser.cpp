/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 22:00:55 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/03 16:36:43 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.hpp"

void parser(const std::vector<t_token> &tokens, ServerContainer &serverContainer)
{
    std::size_t pos = 0;
    std::string tok;
    BaseBlock baseBlock;
    
    if (tokens.empty())
        throw std::runtime_error("Empty token list");

    expect_token(tokens, pos);

    if (!is_word(tokens[pos]) || tokens[pos].word != "http")
        throw_parse_error("Expected 'http' at start");

    ++pos;
    expect_token(tokens, pos);
    
    if (!is_brace_open(tokens[pos]))
        throw_parse_error("Expected '{' after http");
    // skip the opening brace
    ++pos;
    // parse http block directives
    parse_baseblock(tokens, baseBlock, pos);
    // parse server blocks
    parse_servers(tokens, serverContainer, baseBlock, pos);

    //print servercontainer for debugging
    // print_server_container(serverContainer);
}
