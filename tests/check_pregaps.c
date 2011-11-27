/* check_pregaps.c - Unit tests for unportable libcueify APIs to
 * detect pregaps
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
#include <libcueify/mcn_isrc.h>
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


cueify_msf_t expected_indices[13][2] = {
    {
	/* Pregap of first track not counted */
	{0, 2, 33},
	ZERO
    },
    {
	{2, 4, 65},
	{4, 48, 63}  /* This pregap may not exist on some systems(??) */
    },
    {
	{4, 48, 65},
	{8, 16, 15}
    },
    {
	{8, 17, 23},
	ZERO
    },
    {
	{11, 5, 13},
	{13, 40, 63}
    },
    {
	{13, 41, 60},
	ZERO
    },
    {
	{17, 16, 63},
	{19, 53, 21}
    },
    {
	{19, 53, 70},
	ZERO
    },
    {
	{24, 59, 0},
	ZERO
    },
    {
	{27, 36, 53},
	ZERO
    },
    {
	{30, 18, 60},
	ZERO
    },
    {
	{33, 0, 8},
	ZERO
    },
    {
	{34, 19, 0},
	ZERO
    }
};


uint8_t expected_num_indices[] = {
    1, 2, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1
};


START_TEST (test_indices)
{
    cueify_indices *indices;
    int i, j;

    indices = cueify_indices_new();
    fail_unless(indices != NULL, "Failed to create cueify_indices object");
    for (i = 0; i < 13; i++) {
	fail_unless(cueify_device_read_track_indices(dev, indices,
						     i + 1) == CUEIFY_OK,
		    "Failed to read indices from device");
	fail_unless(cueify_indices_get_num_indices(indices) ==
		    expected_num_indices[i],
		    "Number of indices read was incorrect");
	if (cueify_indices_get_num_indices(indices) ==
	    expected_num_indices[i]) {
	    for (j = 0; j < expected_num_indices[i]; j++) {
		/* Only pregaps */
		if (j == 0) {
		    fail_unless(cueify_indices_get_index_number(indices, j) ==
				1,
				"Index number was incorrect");
		} else {
		    fail_unless(cueify_indices_get_index_number(indices, j) ==
				0,
				"Index number was incorrect");
		}
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


START_TEST (test_mcn_isrc)
{
    char buffer[32];  /* This is a reasonable buffer size */
    size_t size = sizeof(buffer);
    int i;

    fail_unless(cueify_device_read_mcn(dev, buffer, &size) == CUEIFY_OK,
		"Failed to (not) read media catalog number from device");
    fail_unless(size == 16 || size == 14,
		"Media catalog number size incorrect");
    fail_unless(strcmp(buffer, "0000000000000") == 0,
		"Media catalog number not set");

    for (i = 0; i < 13; i++) {
	size = sizeof(buffer);
	fail_unless(cueify_device_read_isrc(dev, i + 1, buffer,
					    &size) == CUEIFY_NO_DATA,
		    "Failed to (not) read ISRC from device");
	fail_unless(size == 1, "ISRC size incorrect");
	fail_unless(strcmp(buffer, "") == 0, "ISRC not empty");
    }
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("pregaps");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    /* Reading indices and MCN/ISRC is slow */
    tcase_set_timeout(tc_core, 60);
    tcase_add_test(tc_core, test_indices);
    tcase_add_test(tc_core, test_mcn_isrc);
    suite_add_tcase(s, tc_core);

    return s;
}


int main() {
    int number_failed;
    Suite *s = toc_suite();
    SRunner *sr = srunner_create(s);

    /*
     * Have chosen the 1987 Sgt. Pepper's as it has both pregaps AND
     * MCN without ISRCs.
     */
    printf("NOTE: These tests are expected to fail except when (certain\n"
	   "      printings of) the 1987 version of \"Sgt. Pepper's\" is \n"
	   "      present in the current computer's CD drive.\n\n");

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
