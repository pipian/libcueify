# check_discid.py - Unit tests for SWIG-based libcueify discid APIs
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
def TOC_TRACK_DESCRIPTOR(adr, ctrl, track, address):
    return [0, (((adr & 0xF) << 4) | (ctrl & 0xF)), track, 0,
            (address >> 24), ((address >> 16) & 0xFF),
            ((address >> 8) & 0xFF), (address & 0xFF)]

# Create a binary track descriptor from a full TOC.
def FULL_TOC_TRACK_DESCRIPTOR(session, adr, ctrl, track, address_min,
                              sec=None, frm=None):
    if sec is None or frm is None:
        min = address_min / 75 / 60
        sec = address_min / 75 % 60 + 2
        frm = address_min % 75
        if sec > 59:
            sec -= 60
            min += 1
    else:
        min = address_min

    return [session, (((adr & 0xF) << 4) | (ctrl & 0xF)), 0, track,
            0, 0, 0, 0, min, sec, frm]

# cdda data
lbas = [33, 9215, 21515, 37148, 49738, 61485, 77613, 89395, 112275,
        124103, 136260, 148358, 154275, 179318]
serialized_cdda_toc = [((8 * len(lbas) + 2) >> 8),
                       ((8 * len(lbas) + 2) & 0xFF),
                       1, len(lbas) - 1]
for i in range(len(lbas)):
    if i == len(lbas) - 1:
        serialized_cdda_toc.extend(TOC_TRACK_DESCRIPTOR(0, 0, 0xAA, lbas[i]))
    else:
        serialized_cdda_toc.extend(TOC_TRACK_DESCRIPTOR(0, 0, i + 1, lbas[i]))
serialized_cdda_sessions = [((10) >> 8), ((10) & 0xFF), 1, 1]
serialized_cdda_sessions.extend(TOC_TRACK_DESCRIPTOR(0, 0, 1, lbas[0]))
serialized_cdda_full_toc = [(((13 + 1 * 3) * 11 + 2) >> 8),
                            (((13 + 1 * 3) * 11 + 2) & 0xFF),
                            1, 1]
for i in range(len(lbas) - 1):
    if i == 0:
        serialized_cdda_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA0, 1,
                                      cueify.SESSION_MODE_1, 0))
        serialized_cdda_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA1, 13, 0, 0))
        serialized_cdda_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA2, lbas[len(lbas) - 1]))
    serialized_cdda_full_toc.extend(
        FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, i + 1, lbas[i]))

# data_first data
lbas = [0, 183381, 186504, 190348, 198331, 208208, 217926, 230854,
	243284, 251868, 262878]
serialized_data_first_toc = [((8 * len(lbas) + 2) >> 8),
                             ((8 * len(lbas) + 2) & 0xFF),
                             1, len(lbas) - 1]
for i in range(len(lbas)):
    if i == len(lbas) - 1:
        serialized_data_first_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, 0, 0xAA, lbas[i]))
    elif i == 0:
        serialized_data_first_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, cueify.TOC_TRACK_IS_DATA, i + 1, lbas[i]))
    else:
        serialized_data_first_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, 0, i + 1, lbas[i]))
serialized_data_first_sessions = [((10) >> 8), ((10) & 0xFF), 1, 1]
serialized_data_first_sessions.extend(
    TOC_TRACK_DESCRIPTOR(0, cueify.TOC_TRACK_IS_DATA, 1, lbas[0]))
serialized_data_first_full_toc = [(((10 + 1 * 3) * 11 + 2) >> 8),
                                  (((10 + 1 * 3) * 11 + 2) & 0xFF),
                                  1, 1]
for i in range(len(lbas) - 1):
    if i == 0:
        serialized_data_first_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA0, 1,
                                      cueify.SESSION_MODE_1, 0))
        serialized_data_first_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA1, 10, 0, 0))
        serialized_data_first_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA2, lbas[len(lbas) - 1]))
        serialized_data_first_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 6, i + 1, lbas[i]))
    else:
        serialized_data_first_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, i + 1, lbas[i]))

# data_last data
lbas = [0, 21445, 34557, 61903, 83000, 98620, 112124, 135655, 154145,
	176766, 194590, 213436, 244076, 258988]
serialized_data_last_toc = [((8 * len(lbas) + 2) >> 8),
                            ((8 * len(lbas) + 2) & 0xFF),
                            1, len(lbas) - 1]
for i in range(len(lbas)):
    if i == len(lbas) - 1:
        serialized_data_last_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, 0, 0xAA, lbas[i]))
    elif i == len(lbas) - 2:
        serialized_data_last_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, cueify.TOC_TRACK_IS_DATA, i + 1, lbas[i]))
    else:
        serialized_data_last_toc.extend(
            TOC_TRACK_DESCRIPTOR(0, 0, i + 1, lbas[i]))
serialized_data_last_sessions = [((10) >> 8), ((10) & 0xFF), 1, 2]
serialized_data_last_sessions.extend(
    TOC_TRACK_DESCRIPTOR(0, cueify.TOC_TRACK_IS_DATA, 13, lbas[12]))
serialized_data_last_full_toc = [(((13 + 2 * 3) * 11 + 2) >> 8),
                                 (((13 + 2 * 3) * 11 + 2) & 0xFF),
                                 1, 2]
for i in range(len(lbas) - 1):
    if i == 0:
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA0, 1,
                                      cueify.SESSION_MODE_1, 0))
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA1, 12, 0, 0))
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, 0xA2, 51, 44, 26))

    if i == len(lbas) - 2:
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA0, 13,
                                      cueify.SESSION_MODE_2, 0))
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA1, 13, 0, 0))
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA2, lbas[len(lbas) - 1]))
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(2, 1, 6, i + 1, lbas[i]))
    else:
        serialized_data_last_full_toc.extend(
            FULL_TOC_TRACK_DESCRIPTOR(1, 1, 4, i + 1, lbas[i]))

CDDA_FREEDB_ID                = 0xc009560d
DATA_FIRST_FREEDB_ID          = 0x9d0db10a
DATA_LAST_FREEDB_ID           = 0xbe0d7d0d
DATA_LAST_LIBDISCID_FREEDB_ID = 0xae0c1e0c

CDDA_MUSICBRAINZ_ID       = "5cVTKwEtQPTiqT4A3ktiYJ14WzQ-"
DATA_FIRST_MUSICBRAINZ_ID = "ry6UkUayKgncrSiv06fnFKJCadM-"
DATA_LAST_MUSICBRAINZ_ID  = "iIqthNFjPeboX2O1GKpqvcQIWDc-"

class TestDiscidFunctions(unittest.TestCase):
    def test_toc_freedb_cdda(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_toc),
                    *serialized_cdda_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_sessions),
                    *serialized_cdda_sessions)))

        self.assertEqual(toc.freedbID(), CDDA_FREEDB_ID)
        self.assertEqual(toc.freedbID(None), CDDA_FREEDB_ID)
        self.assertEqual(toc.freedbID(sessions), CDDA_FREEDB_ID)

    def test_toc_freedb_data_first(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_toc),
                    *serialized_data_first_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_sessions),
                    *serialized_data_first_sessions)))

        self.assertEqual(toc.freedbID(), DATA_FIRST_FREEDB_ID)
        self.assertEqual(toc.freedbID(None), DATA_FIRST_FREEDB_ID)
        self.assertEqual(toc.freedbID(sessions), DATA_FIRST_FREEDB_ID)

    def test_toc_freedb_data_last(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_toc),
                    *serialized_data_last_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_sessions),
                    *serialized_data_last_sessions)))

        self.assertEqual(toc.freedbID(), DATA_LAST_FREEDB_ID)
        self.assertEqual(toc.freedbID(None), DATA_LAST_FREEDB_ID)
        self.assertEqual(toc.freedbID(sessions), DATA_LAST_LIBDISCID_FREEDB_ID)

    def test_full_toc_freedb_cdda(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_full_toc),
                    *serialized_cdda_full_toc)))
        
        self.assertEqual(toc.freedbID(), CDDA_FREEDB_ID)
        self.assertEqual(toc.freedbID(False), CDDA_FREEDB_ID)
        self.assertEqual(toc.freedbID(True), CDDA_FREEDB_ID)

    def test_full_toc_freedb_data_first(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_full_toc),
                    *serialized_data_first_full_toc)))
        
        self.assertEqual(toc.freedbID(), DATA_FIRST_FREEDB_ID)
        self.assertEqual(toc.freedbID(False), DATA_FIRST_FREEDB_ID)
        self.assertEqual(toc.freedbID(True), DATA_FIRST_FREEDB_ID)

    def test_full_toc_freedb_data_last(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_full_toc),
                    *serialized_data_last_full_toc)))
        
        self.assertEqual(toc.freedbID(), DATA_LAST_LIBDISCID_FREEDB_ID)
        self.assertEqual(toc.freedbID(False), DATA_LAST_LIBDISCID_FREEDB_ID)
        self.assertEqual(toc.freedbID(True), DATA_LAST_FREEDB_ID)

    def test_toc_musicbrainz_cdda(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_toc),
                    *serialized_cdda_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_sessions),
                    *serialized_cdda_sessions)))

        self.assertEqual(toc.musicbrainzID(), CDDA_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(None), CDDA_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(sessions), CDDA_MUSICBRAINZ_ID)

    def test_toc_musicbrainz_data_first(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_toc),
                    *serialized_data_first_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_sessions),
                    *serialized_data_first_sessions)))

        self.assertEqual(toc.musicbrainzID(), DATA_FIRST_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(None), DATA_FIRST_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(sessions),
                         DATA_FIRST_MUSICBRAINZ_ID)

    def test_toc_musicbrainz_data_last(self):
        toc = cueify.TOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_toc),
                    *serialized_data_last_toc)))
        sessions = cueify.Sessions()
        self.assertTrue(
            sessions.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_sessions),
                    *serialized_data_last_sessions)))

        self.assertEqual(toc.musicbrainzID(), DATA_LAST_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(None), DATA_LAST_MUSICBRAINZ_ID)
        self.assertEqual(toc.musicbrainzID(sessions), DATA_LAST_MUSICBRAINZ_ID)

    def test_full_toc_musicbrainz_cdda(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_cdda_full_toc),
                    *serialized_cdda_full_toc)))
        
        self.assertEqual(toc.musicbrainzID, CDDA_MUSICBRAINZ_ID)

    def test_full_toc_musicbrainz_data_first(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_first_full_toc),
                    *serialized_data_first_full_toc)))
        
        self.assertEqual(toc.musicbrainzID, DATA_FIRST_MUSICBRAINZ_ID)

    def test_full_toc_musicbrainz_data_last(self):
        toc = cueify.FullTOC()
        self.assertTrue(
            toc.deserialize(
                struct.pack(
                    "B" * len(serialized_data_last_full_toc),
                    *serialized_data_last_full_toc)))
        
        self.assertEqual(toc.musicbrainzID, DATA_LAST_MUSICBRAINZ_ID)

if __name__ == '__main__':
    unittest.main()
