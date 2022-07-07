#!/usr/bin/python
import fileinput
import os
import sys


def printHeader():
	print  ("HTTP/1.1 200 OK")
	print  ("Content-Type: text/html\n")



printHeader()

print( "<h3>")
print( "<pre>")

print( "hello from python script. this was on input: ")
for line in fileinput.input():
	print (line)


print( "queryString: ")
print(os.getenv("QUERY_STRING"))
print( "content type: ")
print(os.getenv("CONTENT_TYPE"))
print( "Method: ")
print(os.getenv("REQUEST_METHOD"))
print( "pathInfo: ")
print(os.getenv("PATH_INFO"))
print( "PATH_TRANSLATED: ")
print(os.getenv("PATH_TRANSLATED"))

print ("</pre>")
print( "</h3>")



# ------WebKitFormBoundaryjj9QGM7Sj9b6ZntI
# Content-Disposition: form-data; name="file"; filename="m.cpp"
# Content-Type: application/octet-stream

# #include <vector>
# #include <iostream>


# int maxSumArr(std::vector<int> arr, int k) {
#         int maxSum = INT_MIN;
#         int currentSum = 0;

#         for (int i = 0; i < arr.size(); i++) {
#                 currentSum += arr[i];
#                 if (i >= k) {
#                         currentSum -= arr[i - k];
#                 }
#                 maxSum = std::max(maxSum, currentSum);
#         }
#         return maxSum;
# }

# int main(void) {
#         std::vector<int> arr(6, 0);
#         arr[0] = 1;
#         arr[1] = 2;

#         std::cout << maxSumArr(arr, 3) << std::endl;
#         return 0;
# }
# ------WebKitFormBoundaryjj9QGM7Sj9b6ZntI--