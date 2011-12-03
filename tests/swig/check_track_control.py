# check_track_control.py - Unit tests for unportable SWIG-based
# libcueify APIs to detect track-level control flags
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
import unittest

class TestTrackControlFunctions(unittest.TestCase):
    def setUp(self):
        self.dev = cueify.Device();
        self.assertTrue(self.dev is not None)
    
    def test_track_control(self):
        toc = self.dev.readTOC()
        self.assertTrue(toc is not None)
        for i in range(7):
            # On Blue Nile, pre-emphasis is marked on the track, but not
            # in the TOC.
            self.assertEqual(toc.tracks[i].controlFlags &
                             cueify.TOC_TRACK_HAS_PREEMPHASIS,
                             0)
            self.assertEqual(self.dev.readTrackControlFlags(i + 1) &
                             cueify.TOC_TRACK_HAS_PREEMPHASIS,
                             cueify.TOC_TRACK_HAS_PREEMPHASIS)
if __name__ == '__main__':
    print ("NOTE: These tests are expected to fail except when (certain\n" +
	  "      early printings of) The Blue Nile's Hats is present in the\n"+
	   "      current computer's CD drive.\n")
    unittest.main()
