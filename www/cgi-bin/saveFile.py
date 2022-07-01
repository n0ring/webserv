# !/usr/bin/python
import fileinput
import os

def getFileName():
	i = 0;
	filename = ""
	fileinput.close()
	for line in fileinput.input():
		print(line)
		if (i == 2):
			fileinput.close()
			return filename
		arr = line.split("=")
		l = len(arr)
		if (arr[0].find("Content-Disposition:") != -1):
			filename = arr[l - 1]
		i = i + 1


def processBlock(boundary):
	filename = getFileName()
	if (len(filename) < 1):
		exit(-1)
	try:
		outfile = open(filename, "w")
	except:
		exit(-1)
	for line in fileinput.input():
		if (line.find(boundary) != -1):
			return
		outfile.write(line)

def printHeader():
	print  ("HTTP/1.1 201 OK")
	print  ("Content-Type: text/html\n")
# main:
printHeader()

print( "<h3>")
print( "<pre>")

contentType = os.getenv("CONTENT_TYPE")
try:
	ctArr = contentType.split()
except:
	exit(-1)
boundary = ""

for el in ctArr:
	if (el.find("boundary") != -1 and el.find("=") != -1):
		boundary = el.split("=")[1]
		break

if len(boundary) < 1:
	exit(-1)

for line in fileinput.input():
	if (line.find(boundary) != -1):
		processBlock(boundary)


print ("</pre>")
print( "</h3>")
