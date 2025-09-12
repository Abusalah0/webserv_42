/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 15:19:11 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/12 03:37:39 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/CGIHandler.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/ServerContainer.hpp"
#include <cstring>
#include <sys/wait.h>

CGIHandler::CGIHandler()
{}

CGIHandler::CGIHandler(ServerContainer* server_container, Client* client):
	m_client(client),
	m_server_container(server_container),
	m_pipe(),
	m_pid(-1),
	m_last_activity()
{
	this->m_pipe[0] = -1;
	this->m_pipe[1] = -1;
}

CGIHandler::~CGIHandler()
{
	clean_handler();
}

void CGIHandler::init_env_map_meta()
{
	HTTPHeader& header = this->m_client->get_request_header();
	const std::pair<std::string, std::string> client_addr = this->m_client->get_client_addr();
	const std::pair<std::string, std::string> server_addr = this->m_client->get_server_addr();
	const std::map<std::string, HTTPHeaderField>& fields = header.get_fields();
	std::map<std::string, HTTPHeaderField>::const_iterator it;
	this->m_env_map["AUTH_TYPE"] = "";
	if (header.get_content_length())
		this->m_env_map["CONTENT_LENGTH"] = ul_to_str(header.get_content_length());
	it = fields.find("content-type");
	if (it != fields.end())
		this->m_env_map["CONTENT_TYPE"] = (*it).second.value;
	else
		this->m_env_map["CONTENT_TYPE"] = "";
	this->m_env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->m_env_map["PATH_INFO"] = header.get_target();
	this->m_env_map["REQUEST_URI"] = header.get_target();
	this->m_env_map["DOCUMENT_ROOT"] = this->m_client->get_document_root();
	this->m_env_map["PATH_TRANSLATED"] = this->m_client->get_path_translated();
	this->m_env_map["QUERY_STRING"] = header.get_query_parameters();
	this->m_env_map["REMOTE_ADDR"] = client_addr.first;
	this->m_env_map["REMOTE_HOST"] = "";
	this->m_env_map["REMOTE_USER"] = "";
	this->m_env_map["REQUEST_METHOD"] = header.get_request_method();
	this->m_env_map["SCRIPT_NAME"] = this->m_client->get_script_name();
	this->m_env_map["SERVER_NAME"] = this->m_client->get_server_name();
	this->m_env_map["SERVER_PORT"] = server_addr.second;
	this->m_env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->m_env_map["SERVER_SOFTWARE"] = SERVER_SOFTWARE;
}

void CGIHandler::init_env_map()
{
	HTTPHeader& header = this->m_client->get_request_header();
	const std::map<std::string, HTTPHeaderField>& fields = header.get_fields();
	init_env_map_meta();

	for (std::map<std::string, HTTPHeaderField>::const_iterator it = fields.begin();
		it != fields.end(); it++)
	{
		const HTTPHeaderField& field = (*it).second;
		if (field.name != "content-type" && is_field_cgi_valid(field.name))
		{
			std::string cgi_name = field.name;
			std::transform(cgi_name.begin(), cgi_name.end(), cgi_name.begin(), chr_to_cgi);
			cgi_name.insert(0, "HTTP_");
			this->m_env_map[cgi_name] = field.value;
		}
	}
}

void delete_darray(char** arr)
{
	for (size_t i = 0; arr[i]; i++)
		delete[] arr[i];
	delete[] arr;
}

char** CGIHandler::generate_env()
{
	char** env;
	size_t env_len = this->m_env_map.size();
	env = new char*[env_len + 1]();
	size_t env_i = 0;
	for (std::map<std::string, std::string>::iterator it = this->m_env_map.begin();
		it != this->m_env_map.end(); it++)
	{
		std::pair<std::string, std::string> entry = *it;
		try
		{
			env[env_i] = new char[entry.first.size() + entry.second.size() + 2];
		}
		catch (const std::exception& e)
		{
			delete_darray(env);
			throw e;
		}
		std::strcpy(env[env_i], entry.first.c_str());
		env[env_i][entry.first.size()] = '=';
		std::strcpy(env[env_i] + entry.first.size() + 1, entry.second.c_str());
		env_i++;
	}
	return env;
}

char** CGIHandler::generate_args(const std::string& cgi_pass, const std::string& full_path)
{
	char** args;

	if (cgi_pass == "executable")
		args = new char*[2]();
	else
		args = new char*[3]();
	try
	{
		if (cgi_pass == "executable")
			args[0] = new char[full_path.size() + 1];
		else
		{
			args[0] = new char[cgi_pass.size() + 1];
			args[1] = new char[full_path.size() + 1];
		}
	}
	catch(const std::exception& e)
	{
		delete_darray(args);
		throw;
	}
	if (cgi_pass == "executable")
		std::strcpy(args[0], full_path.c_str());
	else
	{
		std::strcpy(args[0], cgi_pass.c_str());
		std::strcpy(args[1], full_path.c_str());
	}
	
	return args;
}

void CGIHandler::child_process(const std::string& cgi_pass, const std::string& full_path)
{
	int res;

	this->m_server_container->close_fds();

	size_t pos = full_path.rfind('/');
	std::string dir_path = full_path.substr(0, pos);
	if (chdir(dir_path.c_str()))
		throw WebservExceptions::ExitChild();

	res = dup2(this->m_pipe[0], STDIN_FILENO);
	close(this->m_pipe[0]);
	this->m_pipe[0] = -1;
	if (res == -1)
		throw WebservExceptions::ExitChild();
	res = dup2(this->m_pipe[1], STDOUT_FILENO);
	close(this->m_pipe[1]);
	this->m_pipe[1] = -1;
	if (res == -1)
		throw WebservExceptions::ExitChild();

	char** env = generate_env();
	char** args;
	try
	{
		args = generate_args(cgi_pass, full_path);
	}
	catch(const std::exception& e)
	{
		delete_darray(env);
		throw;
	}

	if (cgi_pass == "executable")
		execve(full_path.c_str(), args, env);
	else
		execve(cgi_pass.c_str(), args, env);
	delete_darray(args);
	delete_darray(env);
	throw WebservExceptions::ExitChild();
}

void CGIHandler::init_cgi(const std::string& cgi_pass, const std::string& full_path)
{
	init_env_map();
	if (pipe(this->m_pipe))
		throw WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	pid_t pid = fork();
	if (pid == -1)
	{
		clean_handler();
		throw WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	}
	if (!pid)
		child_process(cgi_pass, full_path);
	else
	{
		this->m_pid = pid;
		this->m_server_container->add_to_poll(this->m_pipe[0], POLLIN);
		this->m_server_container->add_to_poll(this->m_pipe[1], POLLOUT);
		this->m_last_activity = std::time(0);
	}
}

bool CGIHandler::is_dead()
{
	if (this->m_pid == -1)
		return true;
	int wstatus;
	pid_t pid = waitpid(this->m_pid, &wstatus, WNOHANG);
	if (!pid)
		return false;
	this->m_pid = -1;
	return true;
}

void CGIHandler::clean_handler()
{
	this->m_env_map.clear();
	if (this->m_pid != -1)
	{
		kill(this->m_pid, SIGTERM);
		cgi_term_entry entry;
		entry.soft_term_time = std::time(0);
		entry.pid = this->m_pid;
		this->m_server_container->add_cgi_term_entry(entry);
	}
	this->m_pid = -1;
	if (this->m_pipe[0] != -1)
		close_read();
	if (this->m_pipe[1] != -1)
		close_write();
}

std::string CGIHandler::read_cgi()
{
	char buffer[CHUNK_SIZE + 1];
	ssize_t res = read(this->m_pipe[0], buffer, CHUNK_SIZE);
	if (res == -1)
	{
		clean_handler();
		throw WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	}
	if (res == 0)
		close_read();
	buffer[res] = 0;
	std::string str_buffer = buffer;
	return str_buffer;
}

void CGIHandler::write_cgi(const std::string& str)
{
	ssize_t res = write(this->m_pipe[1], str.c_str(), str.size());
	if (res == -1)
	{
		clean_handler();
		if (errno == EPIPE)
			throw WebservExceptions::HTTPException(HTTP_BAD_GATEWAY);
		throw WebservExceptions::HTTPException(HTTP_INTERNAL_SERVER_ERROR);
	}
}

bool CGIHandler::is_read_ready()
{
	if (this->m_pipe[0] == -1)
		return false;
	pollfd& entry = this->m_server_container->get_poll_entry(this->m_pipe[0]);
	if (entry.revents & POLLIN || entry.revents & POLLHUP)
	{
		this->m_last_activity = std::time(0);
		return true;
	}
	return false;
}

bool CGIHandler::is_write_ready()
{
	if (this->m_pipe[1] == -1)
		return false;
	pollfd& entry = this->m_server_container->get_poll_entry(this->m_pipe[1]);
	if (entry.revents & POLLOUT)
	{
		this->m_last_activity = std::time(0);
		return true;
	}
	return false;
}

void CGIHandler::close_write()
{
	this->m_server_container->remove_from_poll(this->m_pipe[1]);
	close(this->m_pipe[1]);
	this->m_pipe[1] = -1;
}

void CGIHandler::close_read()
{
	this->m_server_container->remove_from_poll(this->m_pipe[0]);
	close(this->m_pipe[0]);
	this->m_pipe[0] = -1;
}

bool CGIHandler::is_write_open()
{
	if (this->m_pipe[1] != -1)
		return true;
	return false;
}

bool CGIHandler::is_read_open()
{
	if (this->m_pipe[0] != -1)
		return true;
	return false;
}

bool CGIHandler::is_timeout()
{
	time_t raw_time = std::time(0);
	if (raw_time >= this->m_last_activity + CGI_TIMEOUT)
		return true;
	return false;
}