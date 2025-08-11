/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbibers <sbibers@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 11:51:22 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/11 12:48:05 by sbibers          ###   ########.fr       */
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
    STRING,
    SYMBOL,
    NUMBER,
    SEMICOLON,
    BRACE_OPEN,
    BRACE_CLOSE
};

typedef struct s_tokenizer
{
    std::string word;
    Type        type;
}  t_tokenizer;
