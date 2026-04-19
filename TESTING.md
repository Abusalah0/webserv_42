# webserv_42 Testing Guide

This guide is for 42 peer-evaluation style testing.

It is written to be practical and repeatable: copy commands, run them, compare expected status codes and behavior.

## 1) Goal of This Guide

Use this document to verify mandatory webserv features from the subject:

- server starts and serves static files
- correct status codes
- GET / POST / DELETE behavior
- upload and body-size limits
- CGI execution
- HTTP/1.0 and HTTP/1.1 compatibility checks
- resilience under concurrent requests

This guide uses curl first (deterministic), then optional netcat/ncat checks.

## 2) Prerequisites

Required tools:

- make
- curl
- python3
- xargs (usually available by default)

Optional tools:

- nc or ncat (raw request testing)

Build once:

```bash
cd /home/abdsalah/webserv_42
make
```

## 3) Use a Known-Good Test Config

Some repository configs are machine-specific or include directives not implemented in this codebase. Use this temporary config for evaluation:

```bash
cat > /tmp/webserv_unified.conf << 'EOF'
http {
    root /home/abdsalah/webserv_42/tester/www;
    server {
        listen localhost:18095;
        index index.html;

        location / { }

        location /post {
            client_max_body_size 1024;
            limit POST;
        }

        location /delete {
            limit DELETE;
        }

        location /cgi_py {
            autoindex on;
            root /home/abdsalah/webserv_42/tester/www/cgi;
            cgi_pass /usr/bin/python3;
            cgi_extension .py;
        }

        location /cgi_exec {
            autoindex on;
            root /home/abdsalah/webserv_42/tests/www/cgi;
            cgi_pass executable;
            cgi_extension .cgi;
        }
    }
}
EOF
```

Make CGI scripts executable:

```bash
chmod +x /home/abdsalah/webserv_42/tests/www/cgi/*.cgi
chmod +x /home/abdsalah/webserv_42/tester/www/cgi/*.sh
```

Start server:

```bash
./webserv /tmp/webserv_unified.conf
```

Open a second terminal for tests.

## 4) Core Mandatory Checks (copy/paste)

### 4.1 Static, redirect, and not-found

```bash
curl -s -o /dev/null -w 'GET / => %{http_code}\n' http://localhost:18095/
curl -s -o /dev/null -w 'GET /dir => %{http_code}\n' http://localhost:18095/dir
curl -s -o /dev/null -w 'GET /dir/ => %{http_code}\n' http://localhost:18095/dir/
curl -s -o /dev/null -w 'GET /not-found => %{http_code}\n' http://localhost:18095/not-found
```

Expected:

- GET / => 200
- GET /dir => 301
- GET /dir/ => 200
- GET /not-found => 404

### 4.2 Method restrictions

```bash
curl -s -o /dev/null -w 'GET /post/file.txt => %{http_code}\n' http://localhost:18095/post/file.txt
curl -s -o /dev/null -w 'DELETE / => %{http_code}\n' -X DELETE http://localhost:18095/
```

Expected:

- GET /post/file.txt => 405
- DELETE / => usually 405 or 4xx depending on your config rules

### 4.3 POST upload and body limit

Small body (allowed):

```bash
curl -s -o /dev/null -w 'POST small => %{http_code}\n' -X POST --data-binary 'hello' http://localhost:18095/post/file.txt
```

Large body (blocked by client_max_body_size 1024):

```bash
python3 - << 'PY' | curl -s -o /dev/null -w 'POST 1025 bytes => %{http_code}\n' -X POST --data-binary @- http://localhost:18095/post/too_big.bin
print('A' * 1025, end='')
PY
```

Expected:

- POST small => 201
- POST 1025 bytes => 413

Chunked POST (important for webserv project):

```bash
printf 'hello' > /tmp/ch_body.txt
curl --http1.1 -H 'Transfer-Encoding: chunked' -s -o /dev/null -w 'POST chunked => %{http_code}\n' --data-binary @/tmp/ch_body.txt http://localhost:18095/post/chunked.txt
```

Expected:

- POST chunked => 201

### 4.4 DELETE behavior (deterministic 204 flow)

Create target file first:

```bash
echo 'delete me' > /home/abdsalah/webserv_42/tester/www/file.txt
curl -s -o /dev/null -w 'DELETE /delete/file.txt => %{http_code}\n' -X DELETE http://localhost:18095/delete/file.txt
ls /home/abdsalah/webserv_42/tester/www/file.txt >/dev/null 2>&1; echo "file_exists=$?"
```

Expected:

- DELETE /delete/file.txt => 204
- file_exists=2 (file removed)

### 4.5 CGI checks

Python CGI validator:

```bash
curl -s -o /tmp/cgi_py_out -w 'CGI py => %{http_code}\n' 'http://localhost:18095/cgi_py/validation_cgi.py?xyzwebservtester=tester&lol=LOL%7A%30&list=abc,123,tester'
head -n 3 /tmp/cgi_py_out
```

Executable CGI:

```bash
curl -s -o /tmp/cgi_exec_out -w 'CGI exec => %{http_code}\n' http://localhost:18095/cgi_exec/env.cgi
head -n 5 /tmp/cgi_exec_out
```

Expected:

- CGI py => 200 and output includes `CGI Validation Success`
- CGI exec => 200 and output shows environment variables in HTML

### 4.6 HTTP version checks (1.0 vs 1.1)

```bash
curl --http1.0 -s -o /dev/null -w 'HTTP/1.0 GET / => %{http_code}\n' http://localhost:18095/
curl --http1.1 -s -o /dev/null -w 'HTTP/1.1 GET / => %{http_code}\n' http://localhost:18095/
```

Expected:

- Both should return a valid response (in this codebase both are 200 for `/`)

Note:

- Many students implement HTTP/1.0 only behavior; this check quickly shows compatibility differences.

## 5) Permission and Error-Handling Checks

### 5.1 Forbidden (403) deterministic test

```bash
chmod 000 /home/abdsalah/webserv_42/tester/www/forbidden
curl -s -o /dev/null -w 'GET /forbidden => %{http_code}\n' http://localhost:18095/forbidden
chmod 644 /home/abdsalah/webserv_42/tester/www/forbidden
```

Expected:

- GET /forbidden => 403

### 5.2 Unknown method

```bash
curl -s -o /dev/null -w 'BAD method => %{http_code}\n' -X BAD http://localhost:18095/
```

Expected:

- Usually 405 (method not allowed) or equivalent controlled error

## 6) Concurrency and Resilience Smoke Test

This is not a full benchmark, but it catches obvious hangs/crashes.

```bash
seq 1 100 | xargs -P20 -I{} curl -s -o /dev/null -w '%{http_code}\n' http://localhost:18095/ | sort | uniq -c
curl -s -o /dev/null -w 'health_after_stress => %{http_code}\n' http://localhost:18095/
```

Expected:

- Mostly/all `200` during burst
- `health_after_stress => 200`
- Server should remain alive and responsive

## 7) Optional Raw Request Tests (nc/ncat)

Raw socket tools vary by system. If they behave oddly, use curl-based checks above as primary proof.

HTTP raw GET example:

```bash
printf 'GET / HTTP/1.1\r\nHost: localhost:18095\r\nConnection: close\r\n\r\n' | nc localhost 18095
```

If your nc variant returns no output, try ncat or add timeout flags depending on your platform.

## 8) Subject-Oriented Evaluation Checklist

Use this as quick scoring before peer evaluation:

- Server starts with config file argument
- No immediate crash under normal and burst traffic
- Correct status codes for common scenarios (200/301/403/404/405/413/201/204)
- GET, POST, DELETE all demonstrated
- Upload works, body-size limit enforced
- CGI works with at least one extension and correct environment
- HTTP/1.0 and HTTP/1.1 requests are both handled
- Server still alive after concurrent requests

## 9) Cleanup Commands

After testing:

```bash
pkill webserv || true
rm -f /tmp/webserv_unified.conf /tmp/ch_body.txt /tmp/cgi_py_out /tmp/cgi_exec_out
rm -f /home/abdsalah/webserv_42/tester/www/file.txt
rm -f /home/abdsalah/webserv_42/tester/www/chunked.txt
rm -f /home/abdsalah/webserv_42/tester/www/post/file.txt
rm -f /home/abdsalah/webserv_42/tester/www/post/chunked.txt
rm -f /home/abdsalah/webserv_42/tester/www/post/too_big.bin
```

## 10) Tips for 42 Peer Evaluators

- Do not test with browser only; browser hides protocol details.
- Always test with explicit expected status codes.
- Test at least one failure path per feature, not only success path.
- Ask student to explain exactly why each response code is returned.
- Ask student to modify one route rule live (for example `limit`) and re-run one command.

This catches copy-paste projects immediately and rewards real understanding.
