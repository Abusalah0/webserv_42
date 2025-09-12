#!/usr/bin/python3

import os
import sys

def parse_version(version):
    if len(version) != 3:
        raise ValueError
    if version[1] != '.':
        raise ValueError
    major = int(version[0])
    minor = int(version[2])
    return major, minor
    

metavars_list = [
    "CONTENT_LENGTH",
    "GATEWAY_INTERFACE",
    "PATH_INFO",
    "PATH_TRANSLATED",
    "QUERY_STRING",
    "REMOTE_ADDR",
    "REQUEST_METHOD",
    "SCRIPT_NAME",
    "SERVER_NAME",
    "SERVER_PORT",
    "SERVER_PROTOCOL",
    "SERVER_SOFTWARE"
]

enforced_metavars_list = [
    "GATEWAY_INTERFACE",
    "PATH_INFO",
    "PATH_TRANSLATED",
    "REMOTE_ADDR",
    "REQUEST_METHOD",
    "SCRIPT_NAME",
    "SERVER_PORT",
    "SERVER_PROTOCOL",
    "SERVER_SOFTWARE"
]

metavars_dict = {}

def return_error(desc):
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/plain")
    print("")
    print(desc)
    sys.exit(1)

for metavar in metavars_list:
    if metavar not in os.environ:
        metavars_dict[metavar] = None
    else:
        metavars_dict[metavar] = os.environ[metavar]

for metavar in enforced_metavars_list:
    if not metavars_dict[metavar]:
        return_error(f"{metavar} is not set")

if metavars_dict["CONTENT_LENGTH"]:
    try:
        content_length = int(metavars_dict["CONTENT_LENGTH"])
        if content_length < 1:
            raise ValueError
    except ValueError:
        return_error("CONTENT_LENGTH is invalid")

try:
    gateway_interface = metavars_dict["GATEWAY_INTERFACE"]
    if gateway_interface[:4] != "CGI/":
        raise ValueError
    major, minor = parse_version(gateway_interface[4:])
    if major != 1 or minor < 0 or minor > 1:
        raise ValueError
except ValueError:
    return_error("GATEWAY_INTERFACE is invalid")

if metavars_dict["PATH_INFO"] != "/cgi_py/validation_cgi.py":
    return_error("PATH_INFO is invalid")

index = metavars_dict["PATH_TRANSLATED"].rfind("/")
document_root = metavars_dict["PATH_TRANSLATED"][:index]
if os.getcwd() != document_root:
    return_error(f'{metavars_dict["PATH_TRANSLATED"]} is invalid')

if metavars_dict["QUERY_STRING"]:
    query_string = metavars_dict["QUERY_STRING"]
    if query_string != "xyzwebservtester=tester&lol=LOL%7A%30&list=abc,123,tester":
        return_error("QUERY_STRING is invalid")

if metavars_dict["SCRIPT_NAME"] != "/cgi_py/validation_cgi.py":
    return_error("SCRIPT_NAME is invalid")
    
try:
    server_port = int(metavars_dict["SERVER_PORT"])
    if server_port < 0 or server_port > 65535:
        raise ValueError
except:
    return_error("SERVER_PORT is invalid")
    
try:
    gateway_interface = metavars_dict["SERVER_PROTOCOL"]
    if gateway_interface[:5] != "HTTP/":
        raise ValueError
    major, minor = parse_version(gateway_interface[5:])
    if major != 1 or minor < 0 or minor > 1:
        raise ValueError
except ValueError:
    return_error("SERVER_PROTOCOL is invalid")

print("Status: 200 OK")
print("Content-Type: text/plain")
print("")
print("CGI Validation Success")
sys.exit(0)