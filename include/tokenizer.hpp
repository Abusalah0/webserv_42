/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 11:51:22 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/19 21:47:40 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

enum Type
{
    WORD,
    SYMBOL,
    NUMBER,
    SEMICOLON,
    BRACE_OPEN,
    BRACE_CLOSE
};

typedef struct s_token
{
    std::string word;
    Type        type;
}  t_token;

std::vector<t_token> tokenize_string(const std::string &buffer);
