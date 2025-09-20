/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:15:44 by sbibers           #+#    #+#             */
/*   Updated: 2025/09/20 14:18:24 by amsaleh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tokenizer.hpp"

static Type find_type(const std::string &str)
// find the type of the word.
{
    if (str == ";")
        return (SEMICOLON);
    else if (str == "{")
        return (BRACE_OPEN);
    else if (str == "}")
        return (BRACE_CLOSE);
    bool all_digits = true;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
        {
            all_digits = false;
            break;
        }
    }
    if (all_digits)
        return (NUMBER);
    return (WORD);
}

std::vector<t_token> tokenize_string(const std::string &buffer)
// tokenize and return all vector (tokens).
{
    std::vector<t_token> vector_tokens;
    t_token token;
    std::string current;
    
    for (size_t i = 0; i < buffer.length(); i++)
    {
        char c = buffer[i];
        if (c == '#') // handle comment.
        {
            if (!current.empty())
            {
                token.word = current;
                token.type = find_type(current);
                vector_tokens.push_back(token);
                current.clear();
            }
            size_t new_line_pos = buffer.find('\n', i);
            if (new_line_pos == std::string::npos)
                break; // EOF
            i = new_line_pos;
            continue;
        }
        else if (c == '{' || c == '}' || c == ';') // handle special char.
        {
            if (!current.empty())
            {
                token.word = current;
                token.type = find_type(current);
                vector_tokens.push_back(token);
                current.clear();
            }
            token.word = buffer[i];
            token.type = find_type(token.word);
            vector_tokens.push_back(token);
        }
        else if (isspace(static_cast<unsigned char>(c))) // handle spaces (whitspaces).
        {
            if (!current.empty())
            {
                token.word = current;
                token.type = find_type(current);
                vector_tokens.push_back(token);
                current.clear();
            }
        }
        else
        {
            current.push_back(c);
        }
    }
    if (!current.empty())
    {
        token.word = current;
        token.type = find_type(current);
        vector_tokens.push_back(token);
    }
    return (vector_tokens);
}
