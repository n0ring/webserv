
import re
import sys
import os
from pathlib import Path

#FILENAME = "_.txt"
#SERVER_READ_PATH = Path("/Users/jkassand/4webserv/test copy 2/www/server1/upload/")
# SERVER_WRITE_PATH = Path("/Users/jkassand/4webserv/test copy 2/www/server1/upload")
# SERVER_WRITE_PATH = Path("/Users/jkassand/4webserv/test copy 3/www/server1/upload")
SERVER_WRITE_PATH = Path(os.environ['PATH_TRANSLATED'])


# print(os.environ['CONTENT_TYPE'])

def parse_file():

    # content_type = "CONTENT_TYPE=multipart/form-data; boundary=------WebKitFormBoundaryd1xz0AI76g1urKJw"

    content_type = os.environ['CONTENT_TYPE']

    # print(content_type)

    boundary = None
	# Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryBU4fQNEaPl2YCys0
    match = re.match("multipart/form-data; boundary=(.*)", content_type)
    if match:
        boundary = match.group(1).strip().encode()

    # print(boundary)
    if not boundary:
        raise Exception("Error: boundary is not set")

    file_content = sys.stdin.buffer.read()

    #with open(SERVER_READ_PATH / FILENAME, "rb") as f:
    #    file_content = f.read()

    parts = file_content.split(boundary)
    if len(parts) < 3:
        raise Exception("Error: no boundary in file")

    is_lf_mode = False

    file_content = parts[1]
    parts = file_content.split(b"\r\n\r\n", 1)
    if len(parts) != 2:
        is_lf_mode = True
        parts = file_content.split(b"\n\n", 1)
        if len(parts) != 2:
            raise Exception("Bad file content")

    raw_headers, body = parts
    headers = [x.strip() for x in raw_headers.split(b"\n")]
    headers = [x for x in headers if x]
    for header in headers:
        header_parts = header.split(b": ", 1)
        if len(header_parts) != 2:
            continue
        key, value = header_parts
        if key.lower() == b"content-disposition":
            content_type_name = None
            content_type_filename = None
            match = re.match(rb'form-data; name="(.*)"; filename="(.*)"', value)
            if match:
                content_type_name = match.group(1)
                content_type_filename = match.group(2)

    if not content_type_filename:
        raise Exception("Error: filename is not set in Content-Type header")

    if is_lf_mode:
        if body.endswith(b"\n"):
            body = body[:-1]
    else:
        if body.endswith(b"\r\n"):
            body = body[:-2]

    if is_lf_mode:
        if body.endswith(b"\n--"):
            body = body[:-3]
    else:
        if body.endswith(b"\r\n--"):
            body = body[:-4]

    # if body.endswith(b"--"):
    #     body = body[:-4]

    with open(str(SERVER_WRITE_PATH).encode() + b"/" +  content_type_filename, "wb") as f:
        f.write(body)

try:
    parse_file()
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print("")
    print("<center><br></br>")
    print("<h1>Success!</h1>")
    print("</center>")
except Exception as e:
    print("Content-Type: text/html")
    print("")
    print("<center><br></br>")
    print("<h1>Fail...</h1>")
    print("</center>")
    print(e)
    exit(1)