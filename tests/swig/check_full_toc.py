# check_full_toc.py - Unit tests for SWIG-based libcueify full TOC APIs
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

# Create a binary track descriptor from a full TOC.
def TRACK_DESCRIPTOR(session, adr, ctrl, track,
                     abs_min, abs_sec, abs_frm, min, sec, frm):
    return [session, (((adr & 0xF) << 4) | (ctrl & 0xF)), 0, track,
            abs_min, abs_sec, abs_frm, 0, min, sec, frm]

serialized_mock_full_toc = [(((13 + 2 * 3) * 11 + 2) >> 8),
                            (((13 + 2 * 3) * 11 + 2) & 0xFF), 1, 2]
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 0xA0, 0, 0, 0, 1, cueify.SESSION_MODE_1, 0))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 0xA1, 0, 0, 0, 12, 0, 0))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 0xA2, 0, 0, 0, 51, 44, 26))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 1, 0, 0, 0, 0, 2, 0))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 2, 0, 0, 0, 4, 47, 70))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 3, 0, 0, 0, 7, 42, 57))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 4, 0, 0, 0, 13, 47, 28))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 5, 0, 0, 0, 18, 28, 50))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 6, 0, 0, 0, 21, 56, 70))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 7, 0, 0, 0, 24, 56, 74))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 8, 0, 0, 0, 30, 10, 55))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 9, 0, 0, 0, 34, 17, 20))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 10, 0, 0, 0, 39, 18, 66))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 11, 0, 0, 0, 43, 16, 40))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(1, 1, 4, 12, 0, 0, 0, 47, 27, 61))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(2, 1, 6, 0xA0, 0, 0, 0, 13, cueify.SESSION_MODE_2, 0))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(2, 1, 6, 0xA1, 0, 0, 0, 13, 0, 0))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(2, 1, 6, 0xA2, 0, 0, 0, 57, 35, 13))
serialized_mock_full_toc.extend(
    TRACK_DESCRIPTOR(2, 1, 6, 13, 1, 2, 3, 54, 16, 26))

class TestFullTOCFunctions(unittest.TestCase):
    def test_serialization(self):
        # Test both deserialization and serialization (since, unlike
        # in the C code, the Python library does not support directly
        # specifying the mock TOC.
        full_toc = cueify.FullTOC()
        self.assertTrue(
            full_toc.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_full_toc),
                    *serialized_mock_full_toc)))

        s = full_toc.serialize()
        self.assertEqual(full_toc.errorCode, cueify.OK)
        self.assertEqual(len(s), len(serialized_mock_full_toc))
        self.assertEqual(
            s,
            struct.pack(
                "B" * len(serialized_mock_full_toc),
                *serialized_mock_full_toc))
    
    def test_getters(self):
        full_toc = cueify.FullTOC()
        self.assertTrue(
            full_toc.deserialize(
                struct.pack(
                    "B" * len(serialized_mock_full_toc),
                    *serialized_mock_full_toc)))
        self.assertEqual(full_toc.firstSession, 1)
        self.assertEqual(full_toc.lastSession, 2)
        self.assertEqual(len(full_toc.tracks), 13)
        self.assertEqual(full_toc.tracks[0].session, 1)
        self.assertEqual(full_toc.tracks[12].session, 2)
        self.assertEqual(full_toc.tracks[0].controlFlags, 4)
        self.assertEqual(full_toc.tracks[12].controlFlags, 6)
        self.assertEqual(full_toc.tracks[0].subQChannelFormat, 1)
        self.assertEqual(full_toc.tracks[12].subQChannelFormat, 1)
        self.assertEqual(len(full_toc.sessions), 2)
        self.assertEqual(len(full_toc.sessions[0].pseudotracks), 3)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_FIRST_TRACK_PSEUDOTRACK].controlFlags, 4)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_LAST_TRACK_PSEUDOTRACK].controlFlags, 4)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].controlFlags, 4)
        self.assertEqual(full_toc.sessions[1].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].controlFlags, 6)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_FIRST_TRACK_PSEUDOTRACK].subQChannelFormat, 1)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_LAST_TRACK_PSEUDOTRACK].subQChannelFormat, 1)
        self.assertEqual(full_toc.sessions[0].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].subQChannelFormat, 1)
        self.assertEqual(full_toc.sessions[1].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].subQChannelFormat, 1)
        self.assertEqual(full_toc.tracks[0].pointAddress.min, 0)
        self.assertEqual(full_toc.tracks[0].pointAddress.sec, 0)
        self.assertEqual(full_toc.tracks[0].pointAddress.frm, 0)
        self.assertEqual(full_toc.sessions[1].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].pointAddress.min, 0)
        self.assertEqual(full_toc.sessions[1].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].pointAddress.sec, 0)
        self.assertEqual(full_toc.sessions[1].pseudotracks[cueify.FULL_TOC_LEAD_OUT_TRACK].pointAddress.frm, 0)
        self.assertEqual(full_toc.tracks[12].pointAddress.min, 1)
        self.assertEqual(full_toc.tracks[12].pointAddress.sec, 2)
        self.assertEqual(full_toc.tracks[12].pointAddress.frm, 3)
        self.assertEqual(full_toc.tracks[0].address.min, 0)
        self.assertEqual(full_toc.tracks[0].address.sec, 2)
        self.assertEqual(full_toc.tracks[0].address.frm, 0)
        self.assertEqual(full_toc.tracks[12].address.min, 54)
        self.assertEqual(full_toc.tracks[12].address.sec, 16)
        self.assertEqual(full_toc.tracks[12].address.frm, 26)
        self.assertEqual(full_toc.sessions[0].firstTrack, 1)
        self.assertEqual(full_toc.sessions[1].firstTrack, 13)
        self.assertEqual(full_toc.sessions[0].lastTrack, 12)
        self.assertEqual(full_toc.sessions[1].lastTrack, 13)
        self.assertEqual(full_toc.firstTrack, 1)
        self.assertEqual(full_toc.lastTrack, 13)
        self.assertEqual(full_toc.sessions[0].type, cueify.SESSION_MODE_1)
        self.assertEqual(full_toc.sessions[1].type, cueify.SESSION_MODE_2)
        self.assertEqual(full_toc.sessions[1].leadoutAddress.min, 57)
        self.assertEqual(full_toc.sessions[1].leadoutAddress.sec, 35)
        self.assertEqual(full_toc.sessions[1].leadoutAddress.frm, 13)
        self.assertEqual(full_toc.discLength.min, 57)
        self.assertEqual(full_toc.discLength.sec, 35)
        self.assertEqual(full_toc.discLength.frm, 13)
        self.assertEqual(full_toc.tracks[11].length.min, 4)
        self.assertEqual(full_toc.tracks[11].length.sec, 16)
        self.assertEqual(full_toc.tracks[11].length.frm, 40)
        self.assertEqual(full_toc.sessions[1].length.min, 3)
        self.assertEqual(full_toc.sessions[1].length.sec, 18)
        self.assertEqual(full_toc.sessions[1].length.frm, 62)

if __name__ == '__main__':
    unittest.main()
