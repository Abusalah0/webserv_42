import sys
from http.client import HTTPConnection
import random
import requests

# HTTPConnection.debuglevel = 1

url = None
host = None
port = None

YELLOW = "\033[33m"
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"

get_tests_dict = {
    "/": 200,
    "/dir/index.html": 200,
    "/.": 200,
    "/xyz/../": 200,
    "/dir/": 200,
    "/dir/dir/": 200,
    "": 200,
    "/dir": 301,
    "/dir/../dir": 301,
    "/../": 400,
    "/../xyz": 400,
    "/xyz/../..": 400,
    "/forbidden": 403,
    "/xyz": 404,
    "/./xyz": 404,
    "/dir/dir/dir/": 404
}

post_tests_dict = {
    "/": {"data": b"LOL", "code": 501},
    "/post/": {"data": b"ZOMBIE", "code": 501},
    "/dir": {"data": b"ZOMBIE", "code": 301},
    "/post/zombie": {"data": b"ZOMBIE", "code": 201},
    "/post/bald": {"data": b"", "code": 201},
    "/post/random_1kb": {"data": random.randbytes(1024), "code": 201},
    "/post/random_1025b": {"data": random.randbytes(1025), "code": 413},
    "/post/forbidden": {"data": b"FORBIDDEN", "code": 403}
}

post_validation_tests_dict = {
    "/zombie": {"data": post_tests_dict["/post/zombie"]["data"], "code": 200},
    "/bald": {"data": post_tests_dict["/post/bald"]["data"], "code": 200},
    "/random_1kb": {"data": post_tests_dict["/post/random_1kb"]["data"], "code": 200},
    "/random_1025b": {"data": post_tests_dict["/post/random_1025b"]["data"], "code": 404},
    "/forbidden": {"data": post_tests_dict["/post/forbidden"]["data"], "code": 403}
}

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
        return resp.read()
    except:
        print_test_fail("GET", test_desc, "Request failed.")

def run_test_post(test_desc, route, code, body):
    try:
        conn = HTTPConnection(host, int(port))
        conn.request("POST", route, headers={
            "Host": "www.tester.com",
            "User-Agent": "webserv-tester/1.0",
            "Content-Length": str(len(body)),
            "Content-Type": "application/octet-stream"
            }, body=body, encode_chunked=False)
        resp = conn.getresponse()
        print_test_result_code("POST", test_desc, resp.status, code)
    except:
        print_test_fail("POST", test_desc, "Request failed.")

def run_test_compare(resp_body: bytes, exp_body: bytes):
    if resp_body != exp_body:
        print(f"{RED}Compare response body with post request body failed{RESET}.")
    else:
        print(f"{GREEN}Compare response body with post request body succeded{RESET}.")

def test_get():
    print(f"{YELLOW}GET tests{RESET}:")
    for key in get_tests_dict:
        run_test_get(f"route {key}", key, get_tests_dict[key])
    # run_test_get("route /", "/", 200)
    # run_test_get("route /dir/index.html", "/dir/index.html", 200)
    # run_test_get("route /.", "/.", 200)
    # run_test_get("route /xyz/../", "/xyz/../", 200)
    # run_test_get("route /dir/", "/dir/", 200)
    # run_test_get("route /dir/dir/", "/dir/dir/", 200)
    # run_test_get("route ''", "", 200)
    # run_test_get("route /dir", "/dir", 301)
    # run_test_get("route /dir/../dir", "/dir/../dir", 301)
    # run_test_get("route /../", "/../", 400)
    # run_test_get("route /../xyz", "/../xyz", 400)
    # run_test_get("route /xyz/../..", "/../xyz", 400)
    # run_test_get("route /forbidden", "/forbidden", 403)
    # run_test_get("route /xyz", "/xyz", 404)
    # run_test_get("route /./xyz", "/./xyz", 404)
    # run_test_get("route /dir/dir/dir/", "/dir/dir/dir/", 404)
    
def test_post():
    print(f"{YELLOW}POST tests{RESET}:")
    for key in post_tests_dict:
        data = post_tests_dict[key]["data"]
        code = post_tests_dict[key]["code"]
        run_test_post(f"route {key}", key, code, data)
        
def test_post_validation():
    print(f"{YELLOW}POST validation tests{RESET}:")
    for key in post_validation_tests_dict:
        data = post_validation_tests_dict[key]["data"]
        code = post_validation_tests_dict[key]["code"]
        body = run_test_get(f"route {key}", key, code)
        if code == 200:
            run_test_compare(body, data)

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
test_post_validation()