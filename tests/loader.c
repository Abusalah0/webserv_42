#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void return_error(const char* msg)
{
	printf(
		"Status: 500 Internal Server Error\n"
		"Content-Type: text/plain\n"
		"\n%s\n",
		msg
	);
	fflush(stdout);
	exit(EXIT_FAILURE);
}

int main()
{
	struct stat statbuf;
	if (stat("index.html", &statbuf))
		return_error("Failed to stat index.html");
	int fd = open("index.html", O_RDONLY);
	if (fd == -1)
		return_error("Failed to open index.html");
	char* buf = malloc(statbuf.st_size);
	if (!buf)
	{
		close(fd);
		return_error("malloc failed");
	}
	ssize_t bytes_read = read(fd, buf, statbuf.st_size);
	close(fd);
	if (bytes_read == -1)
	{
		free(buf);
		return_error("read failed");
	}
	printf(
		"Status: 200 OK\n"
		"Content-Type: text/html\n"
		"\n"
	);
	fflush(stdout);
	write(STDOUT_FILENO, buf, bytes_read);
	return 0;
}