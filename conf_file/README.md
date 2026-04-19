# Configuration Directives Guide

This document explains the configuration format accepted by this project, the meaning of each directive, and how values are resolved across http, server, and location blocks.

The parser is strict: unknown directives, missing semicolons, malformed blocks, and invalid values cause startup failure.

## 1. Configuration Shape

The file must start with an http block.

```nginx
http {
    # http-level directives

    server {
        # server-level directives

        location /path {
            # location-level directives
        }
    }
}
```

## 2. Context and Inheritance

Configuration values are inherited from parent to child:

- http -> server -> location
- A directive set in a child context overrides the inherited value.

Practical behavior in this codebase:

- root, autoindex, client_max_body_size: child assignment overrides parent value.
- error_page: child can define/override per status code.
- index: first index directive in a context replaces inherited index list, then additional index directives in the same context append.
- limit (location): if absent, all methods are allowed.

## 3. Directive Reference

### 3.1 Shared directives (http, server, location)

#### root

```nginx
root <path>;
```

Defines the filesystem root used for path resolution.

- If the path does not start with /, the server prefixes it with /var/lib/webserv/.
- Trailing slash is removed internally.

---

#### autoindex

```nginx
autoindex on;
autoindex off;
```

Enables or disables directory listing generation when requesting a directory.

Default: off

---

#### index

```nginx
index <file1> [file2 ...];
```

Defines index files to try when the target is a directory.

Example:

```nginx
index index.html index.htm home.html;
```

---

#### error_page

```nginx
error_page <code1> [code2 ...] <page_or_redirect>;
```

Maps one or more HTTP status codes to a custom page path or redirect target.

- Accepted codes: 300 through 599.
- If page_or_redirect starts with /, it is treated as a file path under root.
- Otherwise, it is treated as a redirect target string.

Example:

```nginx
error_page 404 403 /errors/not_found.html;
error_page 500 502 504 /errors/50x.html;
```

---

#### client_max_body_size

```nginx
client_max_body_size <number>[k|m|g];
```
Maximum accepted request body size.

Rules:

- Integer only (no decimal point).
- Unit suffix is optional and case-insensitive.
- Supported suffixes: k, m, g.

Examples:

```nginx
client_max_body_size 1024;
client_max_body_size 1m;
client_max_body_size 2G;
```

Default: 1m

### 3.2 Server directives

#### listen

```nginx
listen <host>:<port>;
```
Adds a listening endpoint for this server.

Examples:

```nginx
listen 0.0.0.0:1090;
listen localhost:8080;
```

- host:port format is required.
- Multiple listen directives are allowed.
- Current implementation is IPv4-oriented.

---

#### server_name

```nginx
server_name <name1> [name2 ...];
```
Defines virtual host names matched against the Host header.

If server_name is not set, the server accepts any host for its listen pair.
---

#### default_server

```nginx
default_server;
```
Marks this server as fallback when host-based matching does not find a specific server.

Rules:

- Use at most one default_server across all server blocks.

### 3.3 Location directives

#### limit

```nginx
limit <METHOD1> [METHOD2 ...];
```
Restricts allowed methods in this location.

Important:

- Method matching is case-sensitive in runtime checks.
- Use uppercase method names such as GET POST DELETE.

Example:

```nginx
limit GET POST;
```

---

#### cgi_pass

```nginx
cgi_pass <interpreter_path_or_executable_mode>;
```
Enables CGI execution for this location.

Accepted forms:

- Absolute interpreter path, for example /usr/bin/python3
- executable (special mode: execute target directly)

---

#### cgi_extension

```nginx
cgi_extension <extension>;
```
Limits CGI execution to targets ending with this extension.

Example:

```nginx
cgi_extension .py;
```

If cgi_pass is set and cgi_extension is not set, CGI may match any target in that location.

## 4. Full Example

```nginx
http {
    root /home/user/www;
    autoindex off;
    client_max_body_size 10m;
    error_page 404 /errors/404.html;

    server {
        listen 0.0.0.0:1090;
        server_name www.example.local;
        default_server;
        index index.html index.htm;

        location / {
            autoindex on;
        }

        location /upload {
            limit POST DELETE;
            client_max_body_size 2m;
        }

        location /cgi_py {
            root /home/user/www/cgi;
            limit GET POST;
            cgi_pass /usr/bin/python3;
            cgi_extension .py;
        }

        location /cgi_exec {
            root /home/user/www/cgi;
            limit GET POST;
            cgi_pass executable;
            cgi_extension .cgi;
        }
    }
}
```

## 5. Common Parse Errors

Use this checklist if startup fails during parsing:

- Missing semicolon at end of a directive.
- Unknown directive name.
- location declared without path.
- listen without host:port format.
- autoindex value other than on/off.
- client_max_body_size with invalid format (for example 1.5m).
- error_page without valid status code list.

## 6. Runtime Notes

- The executable is started with one config path:

```bash
./webserv ./conf_file/conf_tester.txt
```

- Parse or validation errors are printed and startup exits.
