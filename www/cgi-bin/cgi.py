import fileinput
import os

print  "HTTP/1.1 200 OK"
print  "Content-Type: text/html\n"

print "<h3>"
print "<pre>"

print "hello from python script. this was on input: "
for line in fileinput.input():
    print line
print "queryString: "
print os.getenv("QUERY_STRING")

print "</pre>"
print "</h3>"