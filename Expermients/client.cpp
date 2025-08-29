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
    "DELETE /cgi-bin/test.sh HTTP/1.1\r\n"
    "User-Agent: LOL1AAFEFEEFEFEF:EFEF \r\n"
	"Host: www.example.com:9000\r\n"
    "Connection:\tkeep-alive clOse lol\t\r\n"
    "Accept: en-US;;\r\n"
	"Content-Length: 0\r\n"
	"Content-Type: lol/lol; charset=UTF-8; lol/lol\r\n"
	"Date: lol\r\n"
	"ETag: 214214;wqf qffwq"
	"Cookie: sessionId,abc123;      theme=dark; foo=bar\r\n"
	"Cookie: sessionId,abc123;      theme=dark; foo=bar\r\n"
	"Expires: lol\r\n"
	"Authorization: iwqhiwdhi;wdjhiqwdjhi\r\n"
    "\r\n";
    sockaddr_in server_addr = {};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    hostent* hostent_ptr = gethostbyname("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9001);
    memcpy((char *)&server_addr.sin_addr.s_addr,
        (char *)hostent_ptr->h_addr,
        hostent_ptr->h_length);
    connect(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    send(fd, request, strlen(request), 0);
    char buf[10001];
    ssize_t bytes_read = recv(fd, buf, 10000, 0);
    buf[bytes_read] = 0;
    printf("%s\n", buf);
    
    return 0;
}