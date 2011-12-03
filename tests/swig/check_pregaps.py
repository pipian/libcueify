# check_pregaps.py - Unit tests for unportable SWIG-based libcueify APIs to
# detect pregaps
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

expected_indices = [
    [
	# Pregap of first track not counted
	cueify.MSFAddress(0, 2, 33)
        ],
    [
	cueify.MSFAddress(2, 4, 65),
	cueify.MSFAddress(4, 48, 63)
        ],
    [
	cueify.MSFAddress(4, 48, 65),
	cueify.MSFAddress(8, 16, 15)
        ],
    [
	cueify.MSFAddress(8, 17, 23)
        ],
    [
	cueify.MSFAddress(11, 5, 13),
	cueify.MSFAddress(13, 40, 63)
        ],
    [
	cueify.MSFAddress(13, 41, 60)
        ],
    [
	cueify.MSFAddress(17, 16, 63),
	cueify.MSFAddress(19, 53, 21)
        ],
    [
	cueify.MSFAddress(19, 53, 70)
        ],
    [
	cueify.MSFAddress(24, 59, 0)
        ],
    [
	cueify.MSFAddress(27, 36, 53)
        ],
    [
	cueify.MSFAddress(30, 18, 60)
        ],
    [
	cueify.MSFAddress(33, 0, 8)
        ],
    [
	cueify.MSFAddress(34, 19, 0)
        ]
    ]

class TestPregapsFunctions(unittest.TestCase):
    def setUp(self):
        self.dev = cueify.Device();
        self.assertTrue(self.dev is not None)
    
    # The number of frames which an index may be off by and still be "correct"
    EPSILON_FRAMES = 30
    
    def offsetsEqualWithinEpsilon(self, offset1, offset2, epsilon):
        lba1 = (offset1.min * 60 + offset1.sec) * 75 + offset1.frm
        lba2 = (offset2.min * 60 + offset2.sec) * 75 + offset2.frm
        
        return (lba1 - lba2) < epsilon and (lba2 - lba1) < epsilon;
    
    def test_indices(self):
        for i in range(13):
            indices = self.dev.readTrackIndices(i + 1)
            self.assertEqual(len(indices), len(expected_indices[i]))
            if len(indices) == len(expected_indices[i]):
                for j in range(len(expected_indices[i])):
                    # Only pregaps.
                    if j == 0:
                        self.assertEqual(indices[j].number, 1)
                    else:
                        self.assertEqual(indices[j].number, 0)
                    self.assertTrue(self.offsetsEqualWithinEpsilon(
                            indices[j].offset, expected_indices[i][j],
                            TestPregapsFunctions.EPSILON_FRAMES))
    
    def test_mcn_isrc(self):
        mcn = self.dev.readMCN()
        self.assertEqual(mcn, "0000000000000")
        for i in range(13):
            isrc = self.dev.readISRC(i)
            self.assertEqual(isrc, "")

if __name__ == '__main__':
    print ("NOTE: These tests are expected to fail except when (certain\n" +
	   "      printings of) the 1987 version of \"Sgt. Pepper's\" is \n" +
	   "      present in the current computer's CD drive.\n")
    unittest.main()
