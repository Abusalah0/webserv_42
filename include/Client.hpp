#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Server.hpp"
#include <HTTPBuffer.hpp>
#include <RequestHeader.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>

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

class Client
{
	private:
		int m_fd;
		int m_client_status;
		int m_process_state;
		Server* m_base_server;
		Server* m_target_server;
		Location* m_target_location;
		in_addr_t m_ip_addr;
		in_port_t m_port;
		time_t m_last_activity;
		HTTPBuffer m_request_buffer;
		HTTPBuffer m_response_buffer;
		RequestHeader m_header;
		std::string m_body;
		size_t m_chunk_size;
		void process_header();
		void process_body();
		void process_body_chunked_size();
		void process_body_chunked_data();
		void process_body_chunked_end();
	public:
		Client();
		Client(int fd, Server* server, sockaddr_in& client_addr);
		~Client();
		void handle_read();
		void handle_send();
		void process();
		void generate_error(const WebservExceptions::HTTPException& e);
		int get_client_status();
};

#endif