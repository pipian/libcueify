/* check_track_control.c - Unit tests for unportable libcueify APIs to
 * detect track-level control flags
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
#include <libcueify/toc.h>
#include <libcueify/data_mode.h>


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


START_TEST (test_track_control)
{
    cueify_toc *toc;
    int i;

    toc = cueify_toc_new();
    fail_unless(toc != NULL, "Failed to create cueify_toc object");
    fail_unless(cueify_device_read_toc(dev, toc) == CUEIFY_OK,
		"Failed to read TOC from device");
    for (i = 0; i < 7; i++) {
	/*
	 * On Blue Nile, pre-emphasis is marked on the track, but not
	 * in the TOC.
	 */
	fail_unless((cueify_toc_get_track_control_flags(
			 toc, i + 1) & CUEIFY_TOC_TRACK_HAS_PREEMPHASIS) == 0,
		    "Failed to (not) read pre-emphasis flag from TOC");
	fail_unless((cueify_device_read_track_control_flags(
			 dev, i + 1) & CUEIFY_TOC_TRACK_HAS_PREEMPHASIS) ==
		    CUEIFY_TOC_TRACK_HAS_PREEMPHASIS,
		    "Failed to read pre-emphasis flag from track");
    }
    cueify_toc_free(toc);
}
END_TEST


Suite *track_control_suite() {
    Suite *s = suite_create("track_control");
    TCase *tc_core = tcase_create("core");

    tcase_set_timeout(tc_core, 10);
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_track_control);
    suite_add_tcase(s, tc_core);

    return s;
}


int main() {
    int number_failed;
    Suite *s = track_control_suite();
    SRunner *sr = srunner_create(s);

    printf("NOTE: These tests are expected to fail except when (certain\n"
	   "      early printings of) The Blue Nile's Hats is present in the\n"
	   "      current computer's CD drive.\n\n");

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
