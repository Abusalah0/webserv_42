#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Server.hpp"
#include "../include/HTTPBuffer.hpp"
#include "../include/HTTPHeader.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>
#include <poll.h>
#include "../include/Exceptions.hpp"
#include "../include/CGIHandler.hpp"

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
	PROCESS_SELECT_TARGET,
	PROCESS_REQUEST,
	PROCESS_FILE_BODY,
	PROCESS_CGI_BEGINNING,
	PROCESS_CGI_READ,
	PROCESS_FILE_UPLOAD
};

static const std::string str_template = "{template}";

class ServerContainer;

class Client
{
	private:
		int m_listen_fd;
		int m_file_fd;
		size_t m_body_size;
		ClientStatus m_client_status;
		ConnectionTypes m_connection_type;
		int m_process_state;
		const Server* m_base_server;
		const BaseBlock* m_target_block;
		ServerContainer* m_server_container;
		time_t m_last_activity;
		HTTPBuffer m_request_buffer;
		HTTPBuffer m_response_buffer;
		HTTPBuffer m_cgi_buffer;
		HTTPHeader m_req_header;
		HTTPHeader m_resp_header;
		std::string m_body;
		std::string m_script_name;
		std::string m_server_name;
		std::string m_document_root;
		std::string m_path_translated;
		size_t m_chunk_size;
		const std::pair<std::string, std::string>* m_server_addr;
		const std::pair<std::string, std::string> m_client_addr;
		CGIHandler m_cgi_handler;
		bool m_cgi_header_finished;
		void process_header();
		void process_body();
		void process_body_chunked_size();
		void process_body_chunked_data();
		void process_body_chunked_end();
		void process_request();
		void process_request_get();
		void process_request_post();
		void process_request_delete();
		void process_request_any();
		void process_file_body();
		void process_file_upload();
		void select_target();
		void generate_error(ushort code, const std::string& msg, const std::string& location);
		void fallback_generate_error(const std::string& msg, const std::string& location);
		void reset_client_state();
		void close_file();
		void prep_process_file_body(const std::string& file_path, const std::string& msg = HTTP_OK_MSG);
		void serve_autoindex(const std::deque<AutoIndexEntry>& entries);
		void direct_serve(const BaseBlock* location_target);
		void handle_index();
		void handle_cgi();
		void process_cgi_beginning();
		void process_cgi_read();
		void handle_cgi_read_chunked(std::string& data);
		void handle_cgi_read(std::string& data);
		void handle_file_upload();
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
		void handle_read();
		void handle_send();
		void process();
		int get_client_status();
		void generate_redirection();
		time_t get_last_activity() const;
		HTTPHeader& get_request_header();
		const std::pair<std::string, std::string>& get_client_addr() const;
		const std::pair<std::string, std::string>& get_server_addr() const;
		const std::string& get_script_name();
		const std::string& get_server_name();
		const std::string& get_document_root();
		const std::string& get_path_translated();
};

#endif