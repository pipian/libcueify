# check_indices.py - Unit tests for unportable SWIG-based libcueify APIs to
# detect track indices
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
        cueify.MSFAddress(0, 2, 0)
        ],
    [
	cueify.MSFAddress(2, 7, 25)
        ],
    [
	cueify.MSFAddress(5, 5, 0)
        ],
    [
	cueify.MSFAddress(9, 19, 72)
        ],
    [
	cueify.MSFAddress(13, 24, 10),
	cueify.MSFAddress(14, 40, 42),
	cueify.MSFAddress(15, 53, 57),
	cueify.MSFAddress(17, 2, 42),
	cueify.MSFAddress(17, 9, 62),
	cueify.MSFAddress(18, 3, 27)
        ],
    [
	cueify.MSFAddress(18, 25, 5)
        ],
    [
	cueify.MSFAddress(22, 0, 50),
	cueify.MSFAddress(24, 38, 0),
	cueify.MSFAddress(25, 48, 47)
        ],
    [
	cueify.MSFAddress(27, 43, 30)
        ],
    [
	cueify.MSFAddress(30, 23, 35)
        ],
    [
	cueify.MSFAddress(32, 42, 55)
        ],
    [
	cueify.MSFAddress(35, 25, 55)
        ],
    [
	cueify.MSFAddress(39, 06, 72),
	cueify.MSFAddress(40, 02, 37),
	cueify.MSFAddress(41, 23, 60),
	cueify.MSFAddress(42, 55, 57),
	cueify.MSFAddress(44, 16, 37),
	cueify.MSFAddress(45, 40, 12),
	cueify.MSFAddress(46, 51, 15),
	cueify.MSFAddress(47, 56, 47),
	cueify.MSFAddress(48, 46, 5),
	cueify.MSFAddress(49, 23, 2),
	cueify.MSFAddress(51, 10, 65)
        ],
    [
	cueify.MSFAddress(53, 25, 0)
        ],
    [
	cueify.MSFAddress(57, 17, 30)
        ],
    [
	cueify.MSFAddress(59, 11, 22)
        ]
    ]

class TestIndicesFunctions(unittest.TestCase):
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
        for i in range(15):
            indices = self.dev.readTrackIndices(i + 1)
            self.assertEqual(len(indices), len(expected_indices[i]))
            if len(indices) == len(expected_indices[i]):
                for j in range(len(expected_indices[i])):
                    # No pregaps.
                    self.assertEqual(indices[j].number, j + 1)
                    self.assertTrue(self.offsetsEqualWithinEpsilon(
                            indices[j].offset, expected_indices[i][j],
                            TestIndicesFunctions.EPSILON_FRAMES))

if __name__ == '__main__':
    print ("NOTE: These tests are expected to fail except when (certain\n" +
           "      printings of) \"Smashing... Live!\" is present in the\n" +
           "      current computer's CD drive.\n")
    unittest.main()
