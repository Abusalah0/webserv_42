import sys
from http.client import HTTPConnection
import random

# HTTPConnection.debuglevel = 1

url = None
host = None
port = None

YELLOW = "\033[33m"
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"

def print_test_result_code(method, test_desc, resp_code, exp_code):
    if resp_code != exp_code:
        print(f"{RED}Test {method} {test_desc} failed{RESET}: Expected code {exp_code} returned {resp_code}.")
    else:
        print(f"{GREEN}Test {method} {test_desc} succeded{RESET}: Expected code {exp_code} returned {resp_code}.")
    
def print_test_fail(method, test_desc, reason):
    print(f"{RED}Test {method} {test_desc} failed{RESET}: {reason}")

def run_test_get(test_desc, route, code):
    try:
        conn = HTTPConnection(host, int(port))
        conn.request("GET", route, headers={
            "Host": "www.tester.com",
            "User-Agent": "webserv-tester/1.0"
            })
        resp = conn.getresponse()
        print_test_result_code("GET", test_desc, resp.status, code)
        return resp.read().decode("ascii")
    except:
        print_test_fail("GET", test_desc, "Request failed.")

def run_test_post(test_desc, route, code, body):
    try:
        conn = HTTPConnection(host, int(port))
        conn.request("POST", route, headers={
            "Host": "www.tester.com",
            "User-Agent": "webserv-tester/1.0"
            }, body=body, encode_chunked=True)
        resp = conn.getresponse()
        print_test_result_code("POST", test_desc, resp.status, code)
    except:
        print_test_fail("POST", test_desc, "Request failed.")

def run_test_compare(test_desc, resp_body, exp_body):
    if resp_body != exp_body:
        print(f"{RED}Compare {test_desc} failed{RESET}.")
    else:
        print(f"{GREEN}Compare {test_desc} succeded{RESET}.")

def test_get():
    print(f"{YELLOW}GET tests{RESET}:")
    run_test_get("route /", "/", 200)
    run_test_get("route /dir/index.html", "/dir/index.html", 200)
    run_test_get("route /.", "/.", 200)
    run_test_get("route /xyz/../", "/xyz/../", 200)
    run_test_get("route /dir/", "/dir/", 200)
    run_test_get("route /dir/dir/", "/dir/dir/", 200)
    run_test_get("route ''", "", 200)
    run_test_get("route /dir", "/dir", 301)
    run_test_get("route /dir/../dir", "/dir/../dir", 301)
    run_test_get("route /../", "/../", 400)
    run_test_get("route /../xyz", "/../xyz", 400)
    run_test_get("route /xyz/../..", "/../xyz", 400)
    run_test_get("route /forbidden", "/forbidden", 403)
    run_test_get("route /xyz", "/xyz", 404)
    run_test_get("route /./xyz", "/./xyz", 404)
    run_test_get("route /dir/dir/dir/", "/dir/dir/dir/", 404)
    
def test_post():
    print(f"{YELLOW}POST tests{RESET}:")
    run_test_post("route /", "/", 501, "LOL")
    run_test_post("route /post/", "/post/", 501, "ZOMBIE")
    run_test_post("route /post/file", "/file", 201, "ZOMBIE")
    body = run_test_get("route /file", "/file", 200)
    run_test_compare("/file response with request body", body, "ZOMBIE")

if len(sys.argv) != 2:
    print("Usage: webserv_tester <http://hostname:port>")
    exit(1)

url = sys.argv[1]

if url[:7] != "http://":
    print("Unknown schema.")
    exit(1)
if not url[7:]:
    print("Empty hostname.")
    exit(1)
tmp = url[7:].split(":")
host = tmp[0]
if len(tmp) > 1:
    try:
        port = int(tmp[1])
    except:
        print("Invalid port.")
        exit(1)

test_get()
test_post()