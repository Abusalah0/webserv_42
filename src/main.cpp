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
#include "../include/Exceptions.hpp"
#include "../include/BaseBlock.hpp"
#include "../include/ServerContainer.hpp"
#include "../include/Server.hpp"
#include "../include/CommonUtils.hpp"

int g_signum = 0;

//static const std::string read_file(const std::string &file_name)
//{
//    std::ifstream file(file_name.c_str());
//    if (!file.is_open())
//        throw WebservExceptions::FileOpenFailure();
//    std :: stringstream buffer;
//    buffer << file.rdbuf();
//    std::string content = buffer.str();
//    return (content);
//}

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
        return (1);
    }
	signal(SIGINT, signal_handler);
	try
	{
		ServerContainer server_container;
		Server server_a;
		server_a.add_listen("0.0.0.0:2000");
		server_a.add_listen("127.0.0.1:9001");
		server_container.add_server(server_a);
		if (g_signum == SIGINT)
			return 0;
		server_container.setup_webserv();
		server_container.loop();
		//const std::string buffer = read_file(argv[1]);
    	//std::vector<t_tokenizer> tokens = tokenize_string(buffer);
    	//for (size_t i = 0; i < tokens.size(); ++i) // to test vector after tokenizer.
    	//{
    	//    std::cout << "[" << tokens[i].word << "] type: " << tokens[i].type << "\n";
    	//}
		
		// BaseBlock* obj = new BaseBlock();
		// std::set<std::string> codes;
		// std::vector<std::string> indexes;
		// indexes.push_back("...");
		// indexes.push_back("dir");
		// indexes.push_back("index.html");
		// indexes.push_back("/");
		// codes.insert("300");
		// obj->set_root("");
		// obj->set_client_max_body_size("1g");
		// obj->set_auto_index("on");
		// obj->insert_error_page(codes, "/error.html");
		// obj->insert_redirect_page(codes, "www.redirect.com");
		// obj->insert_index_pages(indexes);
		// BaseBlock obj2(*obj);
		// delete obj;
		// std::cout << "Root: " << obj2.get_root() << std::endl;
		// std::cout << "Autoindex: " << obj2.get_auto_index() << std::endl;
		// std::cout << "Max body size: " << obj2.get_client_max_body_size() << std::endl;
		// std::cout << "Index page: " << obj2.get_index_page("/") << std::endl;
		// std::cout << "Error page: " << obj2.get_error_page(300) << std::endl;
		// std::cout << "Redirect page: " << obj2.get_redirect_page(300) << std::endl;
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
    return (0);
}
