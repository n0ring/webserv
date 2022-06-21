import fileinput

print  "Content-Type: text/html\n\n"

print "<h3>"
print "<pre>"

print "hello from python script. this was on input: "
for line in fileinput.input():
    print line

print "</pre>"
print "</h3>"