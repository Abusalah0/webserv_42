/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 01:34:00 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/23 11:09:37 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"

static const std::string read_file(const std::string &file_name)
// check file and read.
{
    std::ifstream file(file_name.c_str());
    if (!file.is_open())
        throw WebservExceptions::FileOpenFailure();
    std :: stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    return (content);
}

int main(int argc, char **argv)
{
	(void)argv;
    if (argc != 2)
    {
        std::cerr << "Error, Use ./webserv ./conf_file/file_name\n";
        return (1);
    }
	try
	{
        ServerContainer serverContainer;
		const std::string buffer = read_file(argv[1]);
    	std::vector<t_token> tokens = tokenize_string(buffer);
    	parser(tokens, serverContainer);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
    return (0);
}
