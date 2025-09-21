/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsaleh <amsaleh@student.42amman.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 15:19:13 by amsaleh           #+#    #+#             */
/*   Updated: 2025/09/21 17:15:31 by amsaleh          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

/**
 * @file CGIHandler.hpp
 * @brief CGI (Common Gateway Interface) script execution and process management.
 * 
 * This class handles the execution of CGI scripts in a secure, controlled environment
 * with proper process isolation and resource management.
 * 
 * Key Features:
 * - Fork-based process isolation for script execution
 * - Pipe-based communication between parent and child processes
 * - Complete CGI environment variable setup (REQUEST_METHOD, QUERY_STRING, etc.)
 * - Process timeout handling and graceful/forceful termination
 * - Proper file descriptor management and resource cleanup
 * - Support for standard CGI/1.1 specification compliance
 * 
 * The CGIHandler ensures secure script execution by:
 * - Running scripts in isolated child processes
 * - Setting up proper environment variables as per CGI specification
 * - Managing stdin/stdout redirection through pipes
 * - Implementing timeout-based process termination
 * - Providing comprehensive error handling and resource cleanup
 * 
 * @see Client, Server, ServerContainer
 */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "CommonUtils.hpp"
# include <map>

class Client;
class Server;
class ServerContainer;

# define CGI_TIMEOUT 10 ///< CGI process timeout in seconds

/**
 * @brief Handles CGI script execution with proper process isolation and resource management.
 * 
 * The CGIHandler class provides a complete implementation of CGI/1.1 specification,
 * enabling secure execution of server-side scripts in isolated child processes.
 * 
 * Core Functionality:
 * - Process forking and execution of CGI interpreters (PHP, Python, etc.)
 * - Bi-directional pipe communication for stdin/stdout redirection
 * - Complete CGI environment variable setup per RFC 3875
 * - Process monitoring with timeout-based termination
 * - Resource cleanup and file descriptor management
 * 
 * Security Features:
 * - Isolated child process execution preventing server compromise
 * - Proper environment variable sanitization and setup
 * - Timeout protection against runaway processes
 * - Safe file descriptor handling with proper cleanup
 * 
 * @note All CGI processes are executed with appropriate resource limits
 * @see Client::process_cgi(), ServerContainer::watch_cgis_term()
 */
class CGIHandler
{
	private:
		Client* m_client;                               ///< Associated client connection
		ServerContainer* m_server_container;            ///< Server container for process management
		int m_in_pipe[2];                               ///< Pipe to send data to child
		int m_out_pipe[2];                              ///< Pipe to read data from child
		pid_t m_pid;                                    ///< Child process ID
		std::map<std::string, std::string> m_env_map;   ///< CGI environment variables
		time_t m_last_activity;                         ///< Last activity timestamp for timeout detection
		
		/// @brief Initialize standard CGI environment variables
		void init_env_map();
		/// @brief Initialize meta-variables specific to HTTP request
		void init_env_map_meta();
		/// @brief Execute CGI script in child process
		void child_process(const std::string& cgi_pass, const std::string& full_path);
		/// @brief Generate environment array for execve()
		char** generate_env();
		/// @brief Generate argument array for execve()
		char** generate_args(const std::string& cgi_pass, const std::string& full_path);

	public:
		// Constructors and destructor
		/// @brief Default constructor
		CGIHandler();
		/// @brief Initialize CGI handler with client and server context
		CGIHandler(ServerContainer* server_container, Client* client);
		/// @brief Destructor with resource cleanup
		~CGIHandler();

		/// @brief Initialize and start CGI process execution
		/// @param cgi_pass Path to CGI interpreter executable
		/// @param full_path Full path to the script file
		void init_cgi(const std::string& cgi_pass, const std::string& full_path);
		
		/// @brief Reset CGI handler state for reuse
		void reset_cgi();
		
		/// @brief Handle child process termination and cleanup
		void handle_death();
		
		/// @brief Clean up handler resources without terminating process
		void clean_handler();
		
		/// @brief Read data from CGI process output
		/// @return String containing CGI output data
		std::string read_cgi();
		
		/// @brief Write data to CGI process input
		/// @param str Data to write to CGI stdin
		void write_cgi(const std::string& str);
		
		/// @brief Check if CGI output is ready for reading
		/// @return true if data available to read
		bool is_read_ready();
		
		/// @brief Check if CGI input is ready for writing
		/// @return true if ready to write data
		bool is_write_ready();
		
		/// @brief Close write end of pipe (CGI stdin)
		void close_write();
		
		/// @brief Close read end of pipe (CGI stdout)
		void close_read();
		
		/// @brief Check if write pipe is still open
		/// @return true if write pipe is open
		bool is_write_open();
		
		/// @brief Check if read pipe is still open
		/// @return true if read pipe is open
		bool is_read_open();
		
		/// @brief Check if CGI process has exceeded timeout
		/// @return true if process should be terminated
		bool is_timeout();
		bool is_dead();
};

#endif
