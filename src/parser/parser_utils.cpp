/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:11:56 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/21 15:06:34 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../include/parser.hpp"

// abdsalah: i will throw better errors later.
// amsaleh: and later never happened
void throw_parse_error(const std::string &msg)
{
    
    throw std::runtime_error("Parse error: " + msg);
}

bool is_word(const t_token &t)  
{
    return (t.type == WORD);
}

bool is_number(const t_token &t)
{
    return (t.type == NUMBER);
}

bool is_semicolon(const t_token &t)
{
    return (t.type == SEMICOLON);
}

bool is_brace_open(const t_token &t)
{
    return (t.type == BRACE_OPEN);
}

bool is_brace_close(const t_token &t)
{
    return (t.type == BRACE_CLOSE);
}

void expect_token(const std::vector<t_token> &tokens, std::size_t pos)
{
    if (pos >= tokens.size())
        throw std::runtime_error("Unexpected end of tokens");
}

bool is_http_directive(const t_token &t)
{
    if (!is_word(t))
        return (false);

    const std::string &word = t.word;

    return (word == "error_page"
        || word == "autoindex"
        || word == "listen"
        || word == "root"
        || word == "index"
        || word == "client_max_body_size"
    );
}
