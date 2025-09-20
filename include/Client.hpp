/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdsalah <abdsalah@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 02:30:29 by abdsalah          #+#    #+#             */
/*   Updated: 2025/09/20 01:19:28 by abdsalah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../include/Server.hpp"
# include "../include/HTTPBuffer.hpp"
# include "../include/HTTPHeader.hpp"
# include <sys/socket.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <ctime>
# include <poll.h>
# include "../include/Exceptions.hpp"
# include "../include/CGIHandler.hpp"

/**
 * @brief Enumeration representing the status of a client connection.
 * Each status indicates a specific state of the client during its interaction with the server.
 * @note CLIENT_ALIVE indicates that the client is active and can send/receive data.
 * @note CLIENT_DONE indicates that the client has completed its request and the connection can be closed
 * if the connection type is not keep-alive.
 * @note CLIENT_ERROR indicates that an error occurred during the client's request processing.
 * @note CLIENT_DISCONNECTED indicates that the client has disconnected from the server.
 */
enum ClientStatus
{
	CLIENT_ALIVE,
	CLIENT_DONE,
	CLIENT_ERROR,
	CLIENT_DISCONNECTED
};

/**
 * @brief Enumeration representing the various states of client request processing.
 * Each state corresponds to a specific phase in handling an HTTP request.
 * @note The states include processing the header, body (including chunked transfer encoding),
 * selecting the target resource, handling the request, serving file bodies, and managing CGI interactions.
 */
enum ClientProcessState
{
	PROCESS_HEADER,
	PROCESS_BODY,
	PROCESS_BODY_CHUNKED_SIZE,
	PROCESS_BODY_CHUNKED_DATA,
	PROCESS_BODY_CHUNKED_END,
	PROCESS_SELECT_TARGET,
	PROCESS_REQUEST,
	PROCESS_FILE_BODY,
	PROCESS_CGI_BEGINNING,
	PROCESS_CGI_READ,
	PROCESS_FILE_UPLOAD
};

static const std::string str_template = "{template}";// to be replaced with actual data

class ServerContainer;

class Client
{
	private:
		int m_listen_fd;// the listening socket fd
		int m_file_fd;// the file fd if any
		size_t m_body_size;// the size of the body read so far
		ClientStatus m_client_status;// the status of the client, alive, done, error, disconnected
		ConnectionTypes m_connection_type;// the connection type, keep-alive or close
		int m_process_state;// the current state of the client processing, header, body, request, file body, cgi beginning, cgi read
		const Server* m_base_server;// the base server selected based on the listening socket
		const BaseBlock* m_target_block;// the target location or server block selected based on the request target
		ServerContainer* m_server_container;// the server container
		time_t m_last_activity;// the last activity timestamp
		HTTPBuffer m_request_buffer;
		HTTPBuffer m_response_buffer;
		HTTPBuffer m_cgi_buffer;
		HTTPHeader m_req_header;
		HTTPHeader m_resp_header;
		std::string m_body;// the body of the request if any
		std::string m_script_name;// the script name if any
		std::string m_server_name;// the server name if any
		std::string m_document_root;// the document root if any
		std::string m_path_translated;// the translated path if any
		size_t m_chunk_size;// the size of the current chunk being processed
		const std::pair<std::string, std::string>* m_server_addr;// the server address (ip, port)
		const std::pair<std::string, std::string> m_client_addr;// the client address (ip, port)
		CGIHandler m_cgi_handler; ///< CGI execution handler for processing scripts
		bool m_cgi_header_finished; ///< Flag indicating if CGI response headers are complete
		
		// Request Processing State Machine
		/**
		 * @brief Processes the HTTP request header from the request buffer.
		 * Parses header fields, validates HTTP version, and extracts request information.
		 */
		void process_header();
		
		/**
		 * @brief Processes the request body for standard (non-chunked) transfers.
		 * Reads content based on Content-Length header.
		 */
		void process_body();
		
		/**
		 * @brief Processes chunk size line in chunked transfer encoding.
		 * Reads and parses the hexadecimal chunk size from the request stream.
		 */
		void process_body_chunked_size();
		
		/**
		 * @brief Processes chunk data in chunked transfer encoding.
		 * Reads the actual chunk content based on the previously parsed chunk size.
		 */
		void process_body_chunked_data();
		
		/**
		 * @brief Processes the end of chunked transfer encoding.
		 * Handles the final CRLF sequence and any trailing headers.
		 */
		void process_body_chunked_end();

		// HTTP Method Handlers
		/**
		 * @brief Main request processing dispatcher.
		 * Routes the request to appropriate method handler based on HTTP method.
		 */
		void process_request();
		
		/**
		 * @brief Handles GET requests for file retrieval and directory listing.
		 * Implements file serving, autoindex generation, and CGI execution.
		 */
		void process_request_get();
		
		/**
		 * @brief Handles POST requests for data submission and file uploads.
		 * Processes form data, file uploads, and CGI script execution.
		 */
		void process_request_post();
		
		/**
		 * @brief Handles DELETE requests for file removal.
		 * Validates permissions and removes specified files from the server.
		 */
		void process_request_delete();
		
		/**
		 * @brief Generic request handler for method validation and routing.
		 * Performs common checks before delegating to specific method handlers.
		 */
		void process_request_any();

		// File and Response Processing
		/**
		 * @brief Processes file content streaming to the client.
		 * Reads file data in chunks and sends it through the response buffer.
		 */
		void process_file_body();
		
		/**
		 * @brief Processes file upload operations.
		 * Handles multipart form data and saves uploaded files to disk.
		 */
		void process_file_upload();

		// Target Selection and Routing
		/**
		 * @brief Selects the target location block based on the request URI.
		 * Matches the request path against configured location blocks.
		 */
		void select_target();

		// Error Handling
		/**
		 * @brief Generates HTTP error responses with custom error pages.
		 * @param code HTTP status code for the error
		 * @param msg Error message to include in response
		 * @param location Location header value for redirects (optional)
		 */
		void generate_error(ushort code, const std::string& msg, const std::string& location);
		
		/**
		 * @brief Fallback error generator for critical errors.
		 * Used when custom error page generation fails.
		 * @param msg Error message to include in response
		 * @param location Location header value for redirects (optional)
		 */
		void fallback_generate_error(const std::string& msg, const std::string& location);

		// State Management
		/**
		 * @brief Resets client state for processing the next request.
		 * Clears buffers, resets counters, and prepares for new request cycle.
		 */
		void reset_client_state();
		
		/**
		 * @brief Closes any open file descriptors associated with the client.
		 * Ensures proper cleanup of file resources.
		 */
		void close_file();

		// Response Generation Helpers
		/**
		 * @brief Prepares file body processing for response generation.
		 * @param file_path Path to the file to be served
		 * @param msg HTTP status message (defaults to "200 OK")
		 */
		void prep_process_file_body(const std::string& file_path, const std::string& msg = HTTP_OK_MSG);
		
		/**
		 * @brief Generates and serves autoindex (directory listing) pages.
		 * @param entries Deque of directory entries to display
		 */
		void serve_autoindex(const std::deque<AutoIndexEntry>& entries);
		
		/**
		 * @brief Directly serves content from a location or server block.
		 * @param location_target Target block containing serving configuration
		 */
		void direct_serve(const BaseBlock* location_target);
		
		/**
		 * @brief Handles index file resolution and serving.
		 * Attempts to serve configured index files in order of preference.
		 */
		void handle_index();

		// CGI Processing
		/**
		 * @brief Initiates CGI script execution.
		 * Sets up environment variables and executes the CGI script.
		 */
		void handle_cgi();
		
		/**
		 * @brief Processes the beginning phase of CGI execution.
		 * Handles initial CGI setup and environment preparation.
		 */
		void process_cgi_beginning();
		
		/**
		 * @brief Processes CGI script output reading.
		 * Reads data from CGI process and handles response formatting.
		 */
		void process_cgi_read();
		
		/**
		 * @brief Handles chunked CGI output processing.
		 * @param data Reference to CGI output data for chunked processing
		 */
		void handle_cgi_read_chunked(std::string& data);
		
		/**
		 * @brief Handles standard CGI output processing.
		 * @param data Reference to CGI output data for standard processing
		 */
		void handle_cgi_read(std::string& data);

		// File Operations
		/**
		 * @brief Handles file upload processing and storage.
		 * Processes multipart uploads and saves files to designated locations.
		 */
		void handle_file_upload();
		
		/**
		 * @brief Handles file deletion requests.
		 * Validates permissions and deletes specified files from the filesystem.
		 */
		void handle_file_delete();
	public:
		Client();
		Client(int fd,
			ServerContainer* server_container,
			Server* server,
			const std::pair<std::string, std::string>* server_addr,
			const std::pair<std::string, std::string>& client_addr
		);
		~Client();

		/**
		 * @brief Handles reading data from the client socket.
		 * Reads data into the request buffer and updates the last activity timestamp.
		 * If an error occurs during reading, the client status is set to CLIENT_ERROR.
		 * @return void
		 */
		void handle_read();

		/**
		 * @brief Handles sending data to the client socket.
		 * Sends data from the response buffer to the client and updates the last activity timestamp.
		 * If an error occurs during sending, the client status is set to CLIENT_ERROR.
		 * @return void
		 */
		void handle_send();

		/**
		 * @brief Processes the client's request based on the current process state.
		 * Handles different states such as processing headers, body, and executing the request.
		 * If an HTTPException occurs, it generates an appropriate error response.
		 * @throws WebservExceptions::HTTPException on HTTP errors during processing.
		 * @return void
		 */
		void process();

		/**
		 * @brief Gets the current status of the client connection.
		 * @return The current ClientStatus (e.g., CLIENT_ALIVE, CLIENT_DONE, CLIENT_ERROR, CLIENT_DISCONNECTED).
		 */
		int get_client_status();
		
		// void generate_redirection();
		/**
		 * @brief Gets the timestamp of the last activity from the client.
		 * @return The time_t value representing the last activity time.
		 */
		time_t get_last_activity() const;

		/**
		 * @brief Gets a reference to the client's HTTP request header as an HTTPHeader object.
		 * @return A reference to the HTTPHeader object representing the client's request header.
		 */
		HTTPHeader& get_request_header();

		/**
		 * @brief Gets the client address (IP and port) that connected to the server.
		 * @return A constant reference to a pair containing the client's IP address and port as strings.
		 */
		const std::pair<std::string, std::string>& get_client_addr() const;

		/**
		 * @brief Gets the server address (IP and port) the client is connected to.
		 * @return A constant reference to a pair containing the server's IP address and port as strings.
		 */
		const std::pair<std::string, std::string>& get_server_addr() const;
		
		/**
		 * @brief Gets the script name associated with the client's request.
		 * @return A constant reference to a string representing the script name.
		 */
		const std::string& get_script_name();

		/**
		 * @brief Gets the server name associated with the client's request.
		 * @return A constant reference to a string representing the server name.
		 */
		const std::string& get_server_name();

		/**
		 * @brief Gets the document root path associated with the client's request.
		 * @return A constant reference to a string representing the document root path.
		 */
		const std::string& get_document_root();

		/**
		 * @brief Gets the translated path associated with the client's request.
		 * @return A constant reference to a string representing the translated path.
		 */
		const std::string& get_path_translated();
};

#endif
