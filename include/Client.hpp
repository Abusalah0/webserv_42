#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Server.hpp"
#include <HTTPBuffer.hpp>
#include <HTTPHeader.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>
#include <poll.h>
#include <Exceptions.hpp>

enum ClientStatus
{
	CLIENT_ALIVE,
	CLIENT_DONE,
	CLIENT_ERROR,
	CLIENT_DISCONNECTED
};

enum ClientProcessState
{
	PROCESS_HEADER,
	PROCESS_BODY,
	PROCESS_BODY_CHUNKED_SIZE,
	PROCESS_BODY_CHUNKED_DATA,
	PROCESS_BODY_CHUNKED_END,
	PROCESS_REQUEST,
	PROCESS_FILE_BODY
};

enum ClientScopes
{
	SCOPE_BASE_SERVER,
	SCOPE_TARGET_SERVER,
	SCOPE_TARGET_LOCATION
};

class ServerContainer;

class Client
{
	private:
		int m_listen_fd;
		int m_file_fd;
		ClientStatus m_client_status;
		int m_process_state;
		ClientScopes m_current_scope;
		const Server* m_base_server;
		const Server* m_target_server;
		const Location* m_target_location;
		ServerContainer* m_server_container;
		time_t m_last_activity;
		HTTPBuffer m_request_buffer;
		HTTPBuffer m_response_buffer;
		HTTPHeader m_header;
		std::string m_body;
		size_t m_chunk_size;
		const std::pair<std::string, std::string>* m_listen_entry;
		void process_header();
		void process_body();
		void process_body_chunked_size();
		void process_body_chunked_data();
		void process_body_chunked_end();
		void process_request();
		void process_file_body();
		void generate_error(ushort code, const std::string& msg, const std::string& location);
		void fallback_generate_error(const std::string& msg, const std::string& location);
		void reset_client_state();
		void close_file();
	public:
		Client();
		Client(int fd,
			ServerContainer* server_container,
			Server* server,
			const std::pair<std::string, std::string>* listen_entry);
		~Client();
		void handle_read();
		void handle_send();
		void handle_index();
		void process();
		int get_client_status();
		void generate_redirection();
		time_t get_last_activity();
};

#endif