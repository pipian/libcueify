/* check_unportable.h - Unit tests for unportable libcueify APIs
 *
 * Copyright (c) 2011 Ian Jacobi <pipian@pipian.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <libcueify/types.h>
#include <libcueify/error.h>
#include <libcueify/device.h>
#include <libcueify/indices.h>


cueify_device *dev;


void setup() {
    dev = cueify_device_new();
    fail_unless(dev != NULL, "Failed to create cueify_device");
    fail_unless(cueify_device_open(dev, NULL) == CUEIFY_OK,
		"Failed to open device");
}


void teardown() {
    fail_unless(cueify_device_close(dev) == CUEIFY_OK,
		"Failed to close device");
    cueify_device_free(dev);
}


/** The number of frames which an index may be off by and still be "correct" */
#define EPSILON_FRAMES 30


int offsets_equal_within_epsilon(cueify_msf_t offset1, cueify_msf_t offset2,
				 int epsilon) {
    int lba1, lba2;

    lba1 = (offset1.min * 60 + offset1.sec) * 75 + offset1.frm;
    lba2 = (offset2.min * 60 + offset2.sec) * 75 + offset2.frm;

    return (lba1 - lba2) < epsilon && (lba2 - lba1) < epsilon;
}


#define ZERO {0, 0, 0}


cueify_msf_t expected_indices[15][11] = {
    {
	{0, 2, 0},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{2, 7, 25},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{5, 5, 0},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{9, 19, 72},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{13, 24, 10},
	{14, 40, 42},
	{15, 53, 57},
	{17, 2, 42},
	{17, 9, 62},
	{18, 3, 27},
	ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{18, 25, 5},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{22, 0, 50},
	{24, 38, 0},
	{25, 50, 33},  /* This is off by 2 seconds from the online report?? */
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{27, 43, 30},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{30, 23, 35},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{32, 42, 55},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{35, 25, 55},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{39, 06, 72},
	{40, 02, 37},
	{41, 23, 60},
	{42, 55, 57},
	{44, 16, 37},
	{45, 40, 12},
	{46, 51, 15},
	{47, 56, 47},
	{48, 46, 5},
	{49, 23, 2},
	{51, 10, 65}
    },
    {
	{53, 25, 0},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{57, 17, 30},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    },
    {
	{59, 11, 22},
	ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO
    }
};


uint8_t expected_num_indices[] = {
    1, 1, 1, 1, 6, 1, 3, 1, 1, 1, 1, 11, 1, 1, 1
};


START_TEST (test_indices)
{
    cueify_indices *indices;
    int i, j;

    indices = cueify_indices_new();
    fail_unless(indices != NULL, "Failed to create cueify_indices object");
    for (i = 0; i < 15; i++) {
	fail_unless(cueify_device_read_track_indices(dev, indices,
						     i + 1) == CUEIFY_OK,
		    "Failed to read indices from device");
	fail_unless(cueify_indices_get_num_indices(indices) ==
		    expected_num_indices[i],
		    "Number of indices read was incorrect");
	if (cueify_indices_get_num_indices(indices) ==
	    expected_num_indices[i]) {
	    for (j = 0; j < expected_num_indices[i]; j++) {
		/* No pregaps. */
		fail_unless(cueify_indices_get_index_number(indices, j) ==
			    j + 1,
			    "Index number was incorrect");
		/*
		 * Note that different approaches to reading track
		 * indices on different OSs/drives lead to different
		 * detected offsets.  We try to account for this by
		 * doing a fuzzy match.
		 */
		fail_unless(offsets_equal_within_epsilon(
				cueify_indices_get_index_offset(indices, j),
				expected_indices[i][j],
				EPSILON_FRAMES),
			    "Index offset was incorrect");
	    }
	}
    }
    cueify_indices_free(indices);
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("indices");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    /* Reading indices is slow */
    tcase_set_timeout(tc_core, 60);
    tcase_add_test(tc_core, test_indices);
    suite_add_tcase(s, tc_core);

    return s;
}


int main() {
    int number_failed;
    Suite *s = toc_suite();
    SRunner *sr = srunner_create(s);

    /*
     * Smashing... Live! is the only multi-index album I have to test with:
     * http://www.hydrogenaudio.org/forums/index.php?showtopic=26286
     *
     * That said, Enigma's "MCMXC a.D." is almost certainly the most COMMON
     * multi-index album, and might benefit others who want to test:
     * http://www.hydrogenaudio.org/forums/index.php?showtopic=17383
     */
    printf("NOTE: These tests are expected to fail except when (certain\n"
	   "      printings of) \"Smashing... Live!\" is present in the\n"
	   "      current computer's CD drive.\n\n");

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
