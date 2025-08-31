#ifndef CLIENT_HPP
#define CLIENT_HPP

//#include "../include/ServerContainer.hpp"
#include "../include/Server.hpp"
#include <HTTPBuffer.hpp>
#include <HTTPHeader.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>
#include <poll.h>

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
	PROCESS_REQUEST
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
		ClientStatus m_client_status;
		int m_process_state;
		ClientScopes m_current_scope;
		Server* m_base_server;
		Server* m_target_server;
		Location* m_target_location;
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
		void generate_error(const WebservExceptions::HTTPException& e);
		void fallback_generate_error(const WebservExceptions::HTTPException& e);
		void reset_client_state();
	public:
		Client();
		Client(int fd,
			ServerContainer* server_container,
			Server* server,
			const std::pair<std::string, std::string>* listen_entry);
		~Client();
		void handle_read();
		void handle_send();
		void process();
		int get_client_status();
};

#endif