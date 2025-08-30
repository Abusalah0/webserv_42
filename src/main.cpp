/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 01:34:00 by abdsalah          #+#    #+#             */
/*   Updated: 2025/08/31 00:31:14 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/tokenizer.hpp"
#include "../include/Exceptions.hpp"
#include "../include/BaseBlock.hpp"
#include "../include/ServerContainer.hpp"
#include "../include/Server.hpp"
#include "../include/CommonUtils.hpp"
#include "../include/parser.hpp"
#include "../include/RequestHeader.hpp"

int g_signum = 0;

static const std::string read_file(const std::string &file_name)
{
   std::ifstream file(file_name.c_str());
   if (!file.is_open())
       throw WebservExceptions::FileOpenFailure();
   std :: stringstream buffer;
   buffer << file.rdbuf();
   std::string content = buffer.str();
   return (content);
}

void signal_handler(int signum)
{
	g_signum = signum;
}

int main(int argc, char **argv)
{
	(void)argv;
    if (argc != 2)
    {
        std::cerr << "Error, Use ./webserv ./conf_file/file_name\n";
        return EXIT_FAILURE;
    }
	signal(SIGINT, signal_handler);
	ServerContainer server_container;
	try
	{
		const std::string buffer = read_file(argv[1]);
    	std::vector<t_token> tokens = tokenize_string(buffer);
    	parser(tokens, server_container);
		if (g_signum == SIGINT)
			return EXIT_SUCCESS;
		server_container.setup_webserv();
		server_container.loop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}