#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "ERROR!" << std::endl;
		return 1;
	}
	struct sockaddr_in serv_addr;
	serv_addr.sin_addr.s_addr = htonl(0xC0A801A7);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(4000);
	memset(serv_addr.sin_zero, 0, 8);
	if (bind(sockfd, (sockaddr *)&serv_addr, sizeof(sockaddr_in)) == -1)
	{
		close(sockfd);
		std::cerr << "ERROR!" << std::endl;
		return 1;
	}
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
	socklen_t addr_len;
	int client_fd = accept(sockfd, &client_addr, &addr_len);
	close(client_fd);
	close(sockfd);
}