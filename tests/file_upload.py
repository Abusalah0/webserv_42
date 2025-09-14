import os
import sys
import requests

def print_error(msg):
    print(msg, file=sys.stderr)

if len(sys.argv) < 3:
    print_error("Usage: file_upload http://hostname:port/route file_path")
    sys.exit(1)
    
url = sys.argv[1]
if url[-1] != '/':
    url += '/'
file_path = sys.argv[2]
file_name = file_path.split("/")[-1]

if not os.path.isfile(file_path):
    print_error(f"{file_path} not found or regular file.")
    sys.exit(1)
    
file_content=""
    
try:
    with open(file_path) as f:
        file_content = f.read()
except Exception as e:
    print_error(f"Error: {e}.")
    sys.exit(1)
    
try:
    resp = requests.post(url + file_name, file_content)
except Exception as e:
    print_error(f"Error: {e}.")
    sys.exit(1)

print(resp.text, end="")