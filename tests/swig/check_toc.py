# check_toc.py - Unit tests for SWIG-based libcueify TOC APIs
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

serialized_mock_toc = [((8 * 14 + 2) >> 8), ((8 * 14 + 2) & 0xFF), 1, 13]
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 1, 0))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 2, 21445))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 3, 34557))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 4, 61903))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 5, 83000))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 6, 98620))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 7, 112124))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 8, 135655))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 9, 154145))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 10, 176766))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 11, 194590))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 12, 213436))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(cueify.SUB_Q_POSITION,
                                            cueify.TOC_TRACK_IS_DATA, 13,
                                            244076))
serialized_mock_toc.extend(TRACK_DESCRIPTOR(0, 0, 0xAA, 258988))

class TestTOCFunctions(unittest.TestCase):
    def test_serialization(self):
        # Test both deserialization and serialization (since, unlike
        # in the C code, the Python library does not support directly
        # specifying the mock TOC.
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_toc),
                    *serialized_mock_toc)))

        s = toc.serialize()
        self.assertEqual(toc.errorCode, cueify.OK)
        self.assertEqual(len(s), len(serialized_mock_toc))
        self.assertEqual(
            s,
            struct.pack(
                "B" * len(serialized_mock_toc),
                *serialized_mock_toc))
    
    def test_getters(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_toc),
                    *serialized_mock_toc)))
        self.assertEqual(toc.firstTrack, 1)
        self.assertEqual(toc.lastTrack, 13)
        self.assertEqual(len(toc.tracks), 13)
        self.assertEqual(toc.tracks[12].controlFlags, cueify.TOC_TRACK_IS_DATA)
        self.assertEqual(toc.leadoutTrack.controlFlags, 0)
        self.assertEqual(toc.tracks[12].subQChannelFormat,
                         cueify.SUB_Q_POSITION)
        self.assertEqual(toc.leadoutTrack.subQChannelFormat,
                         cueify.SUB_Q_NOTHING)
        self.assertEqual(toc.tracks[12].address, 244076)
        self.assertEqual(toc.leadoutTrack.address, 258988)
        self.assertEqual(toc.discLength, 258988)
        self.assertEqual(toc.tracks[12].length, 258988 - 244076)

if __name__ == '__main__':
    unittest.main()
