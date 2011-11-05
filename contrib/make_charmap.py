# make_charmap.py - Create a character map header for use with
# libcueify's charset conversion functions.
#
# Copyright (c) 2010, 2011 Ian Jacobi <pipian@pipian.com>
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

chars = {}
reverse_chars = {}
for file in sys.argv[1:]:
    f = open(file, 'r')
    data = f.read()
    f.close()
    
    for line in data.split("\n"):
        # Remove comments
        if '#' in line:
            line = line[:line.index('#')]
        
        # Remove whitespace
        line = line.strip()
        if line == '':
            continue
        
        # Get pairs.
        other, unicode = line.split()
        other = int(other[2:], 16)
        if '+' in unicode:
            unicode = unicode.split('+')
            unicode[0] = int(unicode[0][2:], 16)
            unicode[1:] = map(lambda x: int(x, 16), unicode[1:])
        else:
            unicode = [int(unicode[2:], 16)]
        
        # Sort by first-byte.
        chars.setdefault((other >> 8), {})[other & 0xFF] = unicode
        reverse_table = reverse_chars
        for char in unicode:
            reverse_table = reverse_table.setdefault((char >> 16), {})
            reverse_table = reverse_table.setdefault((char >> 8) & 0xFF, {})
            if char == unicode[-1]:
                if reverse_table.get(char & 0xFF, None) is None:
                    reverse_table[char & 0xFF] = other
                elif isinstance(reverse_table[char & 0xFF], dict):
                    reverse_table[char & 0xFF][None] = other
                else:
                    # Replace the previous copy.
                    reverse_table[char & 0xFF] = other
            else:
                if reverse_table.get(char & 0xFF, None) is None:
                    reverse_table = reverse_table.setdefault(char & 0xFF, {})
                elif isinstance(reverse_table[char & 0xFF], dict):
                    reverse_table = reverse_table[char & 0xFF]
                else:
                    reverse_table[char & 0xFF] = {
                        None: reverse_table[char & 0xFF]}
                    reverse_table = reverse_table[char & 0xFF]

def formatTable(lobytes, hibyte=None):
    if hibyte is None:
        print "static const char * const table[256] = {"
    else:
        print "static const char * const table%02X[256] = {" % (hibyte)
    line = '    '
    for byte in range(256):
        line += '"'
        if byte in lobytes:
            for char in lobytes[byte]:
                if char < 0x10000:
                    line += ''.join(map(lambda x: '\\x%02X' % (ord(x)),
                                        unichr(char).encode('utf-8')))
                else:
                    hi, lo = divmod(char - 0x10000, 0x400)
                    char = unichr(0xd800 + hi) + unichr(0xdc00 + lo)
                    line += ''.join(map(lambda x: '\\x%02X' % (ord(x)),
                                        char.encode('utf-8')))
        line += '"'
        if byte != 255:
            line += ','
        if byte % 16 != 15:
            line += ' '
        else:
            print line
            line = '    '
    print "};"
    print

def formatMasterTable(hibytes):
    print "static const char * const * const master_table[256] = {"
    line = '    '
    for byte in range(256):
        if byte in hibytes:
            line += "table%02X" % (byte)
        else:
            line += "NULL"
        if byte != 255:
            line += ','
        if byte % 16 != 15:
            line += ' '
        else:
            print line
            line = '    '
    print "};"
    print

formatReverseMasterTable = None

def formatReverseSubsubtable(chars, prefix, hi, mid):
    for lo in chars[hi][mid]:
        if isinstance(chars[hi][mid][lo], dict):
            formatReverseMasterTable(chars[hi][mid][lo],
                                     '%02X%02X%02X' % (hi, mid, lo))
    
    print ("static const struct multibyte_codepoint " +
           "reverse_table%s%02X%02X[256] = {" % (prefix, hi, mid))
    line = '    '
    for lo in range(256):
        if lo in chars[hi][mid]:
            if isinstance(chars[hi][mid][lo], dict):
                if chars[hi][mid][lo].get(None, None) is not None:
                    line += "{1, {0x%02X, 0x%02X}, " % (
                        chars[hi][mid][lo][None] >> 8,
                        chars[hi][mid][lo][None] & 0xFF)
                    line += "reverse_master_table%s%02X%02X%02X}" % (
                        prefix, hi, mid, lo)
                else:
                    line += "{0, {0, 0}, "
                    line += "reverse_master_table%s%02X%02X%02X}" % (
                        prefix, hi, mid, lo)
            else:
                line += "{1, {0x%02X, 0x%02X}, NULL}" % (
                    chars[hi][mid][lo] >> 8,
                    chars[hi][mid][lo] & 0xFF)
        else:
            line += "{0, {0, 0}, NULL}"
        if lo != 255:
            line += ','
        if lo % 16 != 15:
            line += ' '
        else:
            print line
            line = '    '
    print "};"
    print

def formatReverseSubtable(chars, prefix, hi):
    for mid in chars[hi]:
        formatReverseSubsubtable(chars, prefix, hi, mid)

    print ("static const struct multibyte_codepoint " +
           "*reverse_table%s%02X[256] = {" % (prefix, hi))
    line = '    '
    for mid in range(256):
        if mid in chars[hi]:
            line += "reverse_table%s%02X%02X" % (prefix, hi, mid)
        else:
            line += "NULL"
        if mid != 255:
            line += ','
        if mid % 16 != 15:
            line += ' '
        else:
            print line
            line = '    '
    print "};"
    print

def formatReverseMasterTable(chars, prefix=''):
    for hi in chars:
        if hi is None:
            continue
        formatReverseSubtable(chars, prefix, hi)

    print ("static const struct multibyte_codepoint * const " +
           "*reverse_master_table%s[256] = {" % (prefix))
    line = '    '
    for hi in range(256):
        if hi in chars:
            line += "reverse_table%s%02X" % (prefix, hi)
        else:
            line += "NULL"
        if hi != 255:
            line += ','
        if hi % 16 != 15:
            line += ' '
        else:
            print line
            line = '    '
    print "};"
    print

hibytes = sorted(chars.keys())
if len(hibytes) == 1 and hibytes[0] == 0:
    # Just need to print out a single table.
    formatTable(chars[0])
else:
    # Need to print out a table for each hibyte.
    for byte in hibytes:
        formatTable(chars[byte], byte)
    formatMasterTable(hibytes)

# Format reverse table and successor tables
formatReverseMasterTable(reverse_chars)
