# check_sessions.py - Unit tests for SWIG-based libcueify multi-session APIs
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

# KLUDGE to allow tests to work.
import sys
sys.path.insert(0, '../../build/swig/python')

import cueify
import struct
import unittest

# Create a binary track descriptor from a TOC.
def TRACK_DESCRIPTOR(adr, ctrl, track, address):
    return [0, (((adr & 0xF) << 4) | (ctrl & 0xF)), track, 0,
            (address >> 24), ((address >> 16) & 0xFF),
            ((address >> 8) & 0xFF), (address & 0xFF)]

serialized_mock_sessions = [(10 >> 8), (10 & 0xFF), 1, 2]
serialized_mock_sessions.extend(
    TRACK_DESCRIPTOR(cueify.SUB_Q_POSITION, cueify.TOC_TRACK_IS_DATA, 13,
		     244076))

class TestSessionsFunctions(unittest.TestCase):
    def test_serialization(self):
        # Test both deserialization and serialization (since, unlike
        # in the C code, the Python library does not support directly
        # specifying the mock TOC.
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_sessions),
                    *serialized_mock_sessions)))

        s = sessions.serialize()
        self.assertEqual(sessions.errorCode, cueify.OK)
        self.assertEqual(len(s), len(serialized_mock_sessions))
        self.assertEqual(
            s,
            struct.pack(
                "B" * len(serialized_mock_sessions),
                *serialized_mock_sessions))
    
    def test_getters(self):
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_sessions),
                    *serialized_mock_sessions)))
        self.assertEqual(sessions.firstSession, 1)
        self.assertEqual(sessions.lastSession, 2)
        self.assertEqual(sessions.lastSessionControlFlags,
                         cueify.TOC_TRACK_IS_DATA)
        self.assertEqual(sessions.lastSessionSubQChannelFormat,
                         cueify.SUB_Q_POSITION)
        self.assertEqual(sessions.lastSessionTrackNumber, 13)
        self.assertEqual(sessions.lastSessionAddress, 244076)

if __name__ == '__main__':
    unittest.main()
