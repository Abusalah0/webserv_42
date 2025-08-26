#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>
#include <stdio.h>

int main()
{
    const char* request =
    "GET /                    HTTP/1.9\r\n"
    "HoSt:www.example.com\r\n"
    "User-Agent: LOL1\n"
    "Connection: keep-alive\r\n"
    "CoNtEnt-Type:text/lol\r\n"
    "Accept: a\r\n"
    "LOL: lol\r\n"
    "Set-Cookies: sessionId=50; lol=lol\r\n"
    "\r\n\r\n";
    sockaddr_in server_addr = {};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    hostent* hostent_ptr = gethostbyname("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    memcpy((char *)&server_addr.sin_addr.s_addr,
        (char *)hostent_ptr->h_addr,
        hostent_ptr->h_length);
    connect(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    send(fd, request, strlen(request), 0);
    char buf[2049];
    ssize_t bytes_read = recv(fd, buf, 2048, 0);
    buf[bytes_read] = 0;
    printf("%s\n", buf);
    return 0;
}