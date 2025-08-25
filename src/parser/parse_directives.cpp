/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_directives.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 12:36:13 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/25 13:23:35 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

void    parse_root_directive(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t &pos)
{
    expect_token(tokens, pos);
    
    if (!is_word(tokens[pos]))
    {
        throw_parse_error("Expected root path after 'root'");
    }
    baseBlock.set_root(tokens[pos].word);
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

void store_redirect_directive(std::vector<t_token> const &tokens, BaseBlock &baseblock, std::size_t &pos)
{
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
    
    for (std::size_t i = 0; i < codes.size(); ++i)
        baseblock.insert_redirect_page(codes, path);
}
