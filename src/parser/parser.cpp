/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 22:00:55 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/22 01:52:00 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"

void parser(const std::vector<t_token> &tokens, ServerContainer &serverContainer)
{
    std::size_t pos = 0;
    std::string tok;
    BaseBlock baseBlock;
    
    (void)serverContainer; // to avoid unused parameter warning
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
    // parse_servers(tokens, serverContainer, baseBlock, pos); // not implemented yet

    //print baseBlock for debugging
    std::cout << "BaseBlock contents:\n";
    std::cout << "Root: " << baseBlock.get_root() << "\n";
    std::cout << "Auto Index: " << (baseBlock.get_auto_index() ?
        "on" : "off") << "\n";
    std::cout << "Client Max Body Size: " << baseBlock.get_client_max_body_size() << "\n";
    std::cout << "Indexes: ";
    std::cout << baseBlock.get_index_page("/") << "\n";
    std::cout << "Error Pages: ";
    // for (const auto &error : baseBlock.get_error_page(404))
    //     std::cout << error << " ";
    // std::cout << "\nRedirect Pages: ";
    // for (const auto &redirect : baseBlock.get_redirect_page(301))
    //     std::cout << redirect << " ";
    // std::cout << "\n";
}
