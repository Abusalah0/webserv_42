#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <netdb.h>

//uint32_t parse_ip(const std::string& str)
//{

//}

int main()
{
	const char* str = "-1.168.100.4";
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_flags = 0;
	hints.ai_addr = 0;
	hints.ai_addrlen = 0;
	addrinfo* addr_info;
	if (getaddrinfo(str, "4000", &hints, &addr_info))
	{
		std::cerr << "ERROR" << std::endl;
		return 1;
	}
	std::cout << ntohs(((sockaddr_in*)addr_info->ai_addr)->sin_port) << std::endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		freeaddrinfo(addr_info);
		std::cerr << "ERROR!" << std::endl;
		return 1;
	}
	for (addrinfo* ai_ptr = addr_info; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
	{
		if (!bind(sockfd, ai_ptr->ai_addr, ai_ptr->ai_addrlen))
		{
			std::cout << "BIND SUCCESS" << std::endl;
			break;
		}
	}
	freeaddrinfo(addr_info);
	if (listen(sockfd, 1024) == -1)
	{
		close(sockfd);
		std::cerr << "ERROR!" << std::endl;
		return 1;
	}
	int fd = open("Makefile", O_RDONLY);
	if (fd == -1)
	{
		close(sockfd);
		std::cerr << "ERROR!" << std::endl;
		return 1;
	}
	pollfd pollfds[2];
	pollfds[0].fd = sockfd;
	pollfds[0].events = POLLIN | POLLOUT;
	pollfds[1].fd = fd;
	pollfds[1].events = 0;
	int res = poll(pollfds, 1, 10000);
	std::cout << "LOL " << res << std::endl;
	sockaddr client_addr;
	socklen_t addr_len = 0;
	int client_fd = accept(sockfd, &client_addr, &addr_len);
	close(client_fd);
	close(sockfd);
}