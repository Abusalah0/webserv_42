/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbibers <sbibers@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:01:26 by sbibers           #+#    #+#             */
/*   Updated: 2025/08/11 14:17:50 by sbibers          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tokenizer.hpp"

static const std::string read_file(const std::string &file_name)
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
