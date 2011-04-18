import sys

chars = {}
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
    print "static const char * const * const masterTable[256] = {"
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

hibytes = sorted(chars.keys())
if len(hibytes) == 1 and hibytes[0] == 0:
    # Just need to print out a single table.
    formatTable(chars[0])
else:
    # Need to print out a table for each hibyte.
    for byte in hibytes:
        formatTable(chars[byte], byte)
    formatMasterTable(hibytes)
