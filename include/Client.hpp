#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Server.hpp"
#include "../include/RequestBuffer.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>

enum ClientStatus
{
	CLIENT_ALIVE,
	CLIENT_ERROR,
	CLIENT_DISCONNECTED
};

enum ClientReadStates
{
	RECV_HEADER,
	RECV_BODY,
	RECV_DONE
};

// enum ClientSendStates
// {
// 	SEND_,
// };

class Client
{
	private:
		int m_fd;
		int m_client_status;
		int m_read_state;
		int m_write_state;
		Server& m_server;
		in_addr_t m_ip_addr;
		in_port_t m_port;
		time_t m_last_activity;
		RequestBuffer m_request_buffer;
	public:
		Client(int fd, Server& server, sockaddr_in& client_addr);
		~Client();
		void handle_read();
		int get_client_status();
};

#endif