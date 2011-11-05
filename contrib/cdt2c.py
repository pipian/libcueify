# cdt2c.py - Simple converter of CD-Text binary data to C header
#
# Copyright (c) 2011 Ian Jacobi <pipian@pipian.com>
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys

if len(sys.argv) != 2:
    print "Usage: cdt2c.py BINARY-FILE"
    sys.exit(0)
f = open(sys.argv[1])
data = f.read()
f.close()

print "static const unsigned char data[] = {"
line = "    "
for i in range(len(data)):
    if i == 4 or (i - 4) % 18 == 0:
        print line
        line = "    "
    elif i > 4 and (i - 4) % 18 in (4, 16):
        # Part of the data
        print line
        line = "      "
    byte = data[i]
    line += "0x%02x," % (ord(byte))
print line[:-1]
print "};"
print

