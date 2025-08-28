#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

int main()
{
    const char* request =
    "GET / HTTP/1.9.1\r\n"
    "User-Agent: LOL1AAFEFEEFEFEF:EFEF \r\n"
	"Host: www.example.com\r\n"
    "Connection:\tkeep-alive;\t  lol,        lol\t\r\n"
	"Connection: keep-alive\r\n"
    "Accept: en-US;;\r\n"
    "ABC: lol\r\n"
	"Content-Type: lol/lol; charset=UTF-8; lol/lol\r\n"
	"Cookie: sessionId,abc123;      theme=dark; foo=bar\r\n"
	"Cookie: sessionId,abc123;      theme=dark; foo=bar\r\n"
    "\r\n";
    sockaddr_in server_addr = {};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    hostent* hostent_ptr = gethostbyname("10.11.1.4");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    memcpy((char *)&server_addr.sin_addr.s_addr,
        (char *)hostent_ptr->h_addr,
        hostent_ptr->h_length);
    connect(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    send(fd, request, strlen(request) - 2, 0);
    char buf[10001];
    ssize_t bytes_read = recv(fd, buf, 10000, 0);
    buf[bytes_read] = 0;
    printf("%s\n", buf);
    
    return 0;
}