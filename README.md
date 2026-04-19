# webserv_42

webserv_42 is a lightweight HTTP/1.1 web server written in C++98. The project focuses on building core web server behavior from scratch: parsing configuration files, handling concurrent client connections, serving static content, and executing CGI scripts safely through child processes.

Instead of being a framework or library, this repository is a systems programming project that demonstrates socket-level networking, protocol parsing, event-driven architecture, and robust process/resource management.

## Quick Start

### 1) Build

```bash
make
```

This creates the executable `./webserv`.

### 2) Run

```bash
./webserv ./conf_file/conf_tester.txt
```

The binary expects exactly one argument: a configuration file path.

### 3) Test

With the server running, execute:

```bash
python3 tester/webserv_tester.py http://localhost:1090
```

The tester validates request routing, status codes, upload/delete behavior, and CGI output paths.

## What It Supports

The server currently supports GET, POST, and DELETE request handling; static file serving with MIME detection; autoindex generation for directories; host-based server selection; and CGI execution through interpreter path or executable script mode. Internally, it uses `poll` for multiplexed I/O and a state-machine based request lifecycle.

## Configuration Example

```nginx
http {
    root /path/to/www;

    server {
        listen 0.0.0.0:1090;
        index index.html;

        location / {
            autoindex on;
        }

        location /post {
            limit POST;
            client_max_body_size 1m;
        }

        location /cgi_py {
            root /path/to/www/cgi;
            cgi_pass /usr/bin/python3;
            cgi_extension .py;
            autoindex on;
        }
    }
}
```

For full configuration syntax and directive behavior, see [conf_file/README.md](conf_file/README.md).
For a complete evaluator-focused feature test workflow, see [TESTING.md](TESTING.md).

## Available Directives

| Context | Directive |
| --- | --- |
| shared (`http`, `server`, `location`) | `root`, `autoindex`, `index`, `error_page`, `client_max_body_size` |
| `server` | `listen`, `server_name`, `default_server` |
| `location` | `limit`, `cgi_extension`, `cgi_pass` |

## Project Structure

| Path | Purpose |
| --- | --- |
| `src/` | Core runtime implementation |
| `include/` | Header files and interfaces |
| `src/parser/` | Configuration parser logic |
| `conf_file/` | Example configurations |
| `tester/` | Functional python tester + sample web content |
| `tests/` | Extra experiments and validation files |

## Build Targets

```bash
make clean
make fclean
make re
```

## Notes

The current implementation is Linux/POSIX oriented and primarily targets IPv4 sockets. CGI and client inactivity are protected with timeout logic, and response/connection behavior follows the internal HTTP state machine.

