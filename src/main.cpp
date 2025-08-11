/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbibers <sbibers@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:26 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/11 13:30:15 by sbibers          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tokenizer.hpp"

const std::string read_file(const std::string &file_name)
// check file and read.
{
    std::ifstream file(file_name.c_str());
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file '" << file_name << "'\n";
        std::exit(1);
    }
    std :: stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();    
    file.close();
    return (content);
}

Type find_type(const std::string &str)
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
        if (!isdigit(str[i]))
        {
            all_digits = false;
            break;
        }
    }
    if (all_digits)
        return (NUMBER);
    return (WORD);
}

std::vector<t_tokenizer> tokenize_string(const std::string &buffer)
// tokenize and return all vector (tokens).
{
    std::vector<t_tokenizer> vector_tokens;
    t_tokenizer token;
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error, Use ./webserv ./conf_file/file_name\n";
        return (1);
    }
    const std::string buffer = read_file(argv[1]);
    std::vector<t_tokenizer> tokens = tokenize_string(buffer);
    for (size_t i = 0; i < tokens.size(); ++i) // to test vector after tokenizer.
    {
        std::cout << "[" << tokens[i].word << "] type: " << tokens[i].type << "\n";
    }
    return (0);
}
