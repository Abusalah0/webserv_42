/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 15:19:13 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/05 23:12:27 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "CommonUtils.hpp"
#include <map>

class Client;
class Server;
class ServerContainer;

class CGIHandler
{
	private:
		Client* m_client;
		ServerContainer* m_server_container;
		int m_pipe[2];
		pid_t m_pid;
		std::map<std::string, std::string> m_env_map;
		void init_env_map();
		void init_env_map_meta();
		void child_process(const std::string& cgi_pass, const std::string& full_path);
		char** generate_env();
		char** generate_args(const std::string& cgi_pass, const std::string& full_path);
	public:
		CGIHandler();
		CGIHandler(ServerContainer* server_container, Client* client);
		~CGIHandler();
		void init_cgi(const std::string& cgi_pass, const std::string& full_path);
		void reset_cgi();
		bool is_dead();
		void clean_handler();
		std::string read_cgi();
		void write_cgi(const std::string& str);
		bool is_read_ready();
		bool is_write_ready();
		void close_write();
		void close_read();
		bool is_write_open();
		bool is_read_open();
};

#endif