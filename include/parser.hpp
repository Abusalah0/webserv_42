/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 13:11:23 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/22 17:55:52 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "tokenizer.hpp"
# include "BaseBlock.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include "ServerContainer.hpp"
# include <vector>
# include <cstring>
# include <set>
# include <map>
# include <stdexcept>
# include <cstdlib>
# include <iostream>
# include <cstddef>


void    throw_parse_error(const std::string &msg);
void    expect_token(const std::vector<t_token> &tokens, std::size_t pos);
bool    is_word(const t_token &t);
bool    is_number(const t_token &t);
bool    is_semicolon(const t_token &t);
bool    is_brace_open(const t_token &t);
bool    is_brace_close(const t_token &t);
bool    is_http_directive(const t_token &t);

void    parser(const std::vector<t_token> &tokens, ServerContainer &serverContainer);
void    skip_location_block(const std::vector<t_token> &tokens, std::size_t &pos);
void    parse_baseblock(const std::vector<t_token> &tokens, BaseBlock &baseBlock, std::size_t pos);
void    parse_servers( const std::vector<t_token> &tokens, ServerContainer &serverContainer, BaseBlock &baseBlock, size_t pos);

#endif