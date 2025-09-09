import sys
from urllib3.connection import HTTPConnection

url = sys.argv[1]
host = None
port = None

GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"

def print_test_result_code(test_desc, resp_code, exp_code):
    if resp_code != exp_code:
        print(f"{RED}Test {test_desc} failed{RESET}: Expected code {exp_code} returned {resp_code}.")
    else:
        print(f"{GREEN}Test {test_desc} succeded{RESET}: Expected code {exp_code} returned {resp_code}.")
    
def print_test_fail(test_desc, reason):
    print(f"{RED}Test {test_desc} failed{RESET}: {reason}")

def run_test_code(test_desc, route, code):
    try:
        conn = HTTPConnection(host, int(port))
        conn.request("GET", route, headers={
            "Host": "www.tester.com",
            "User-Agent": "webserv-tester/1.0"
            })
        resp = conn.getresponse()
        print_test_result_code(test_desc, resp.status, code)
    except Exception as e:
        print(e)
        print_test_fail(test_desc, "Request failed.")

def test_general():
    print(f"{GREEN}General tests{RESET}:")
    run_test_code("Access route /", "/", 200)
    run_test_code("Access route /dir/index.html", "/dir/index.html", 200)
    run_test_code("Access route /.", "/.", 200)
    run_test_code("Access route /xyz/../", "/xyz/../", 200)
    run_test_code("Access route /dir", "/dir", 301)
    run_test_code("Access route /dir/../dir", "/dir/../dir", 301)
    run_test_code("Access route /../", "/../", 400)
    run_test_code("Access route /../xyz", "/../xyz", 400)
    run_test_code("Access route /xyz/../..", "/../xyz", 400)
    run_test_code("Access route /dir/", "/dir/", 403)
    run_test_code("Access route /xyz", "/xyz", 404)
    run_test_code("Access route /./xyz", "/./xyz", 404)

if len(sys.argv) != 2:
    print("Usage: webserv_tester <http://hostname:port>")
    exit(1)

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

test_general()