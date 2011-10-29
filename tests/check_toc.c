/* check_toc.h - Unit tests for libcueify TOC APIs
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

#include <stdlib.h>
#include <check.h>
#include <libcueify/types.h>
#include <libcueify/error.h>
#include <libcueify/toc.h>
#include "toc_private.h"

/* Create a binary track descriptor from a TOC. */
#define TRACK_DESCRIPTOR(adr, ctrl, track, address) \
    0, (((adr & 0xF) << 4) | (ctrl & 0xF)), track, 0,			\
	(address >> 24), ((address >> 16) & 0xFF),			\
	((address >> 8) & 0xFF), (address & 0xFF)

uint8_t serialized_mock_toc[] = {
    ((8 * 14 + 2) >> 8), ((8 * 14 + 2) & 0xFF), 1, 13,
    TRACK_DESCRIPTOR(0, 0, 1, 0),
    TRACK_DESCRIPTOR(0, 0, 2, 21445),
    TRACK_DESCRIPTOR(0, 0, 3, 34557),
    TRACK_DESCRIPTOR(0, 0, 4, 61903),
    TRACK_DESCRIPTOR(0, 0, 5, 83000),
    TRACK_DESCRIPTOR(0, 0, 6, 98620),
    TRACK_DESCRIPTOR(0, 0, 7, 112124),
    TRACK_DESCRIPTOR(0, 0, 8, 135655),
    TRACK_DESCRIPTOR(0, 0, 9, 154145),
    TRACK_DESCRIPTOR(0, 0, 10, 176766),
    TRACK_DESCRIPTOR(0, 0, 11, 194590),
    TRACK_DESCRIPTOR(0, 0, 12, 213436),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, CUEIFY_TOC_TRACK_IS_DATA, 13,
		     244076),
    TRACK_DESCRIPTOR(0, 0, 0xAA, 258988)
};

cueify_toc_private mock_toc;

void setup() {
    uint32_t lbas[14] = {
	258988, 0, 21445, 34557, 61903, 83000, 98620, 112124, 135655, 154145,
	176766, 194590, 213436, 244076
    };
    int i;

    memset(&mock_toc, 0, sizeof(mock_toc));

    mock_toc.first_track_number = 1;
    mock_toc.last_track_number = 13;
    for (i = 0; i < 14; i++) {
	if (i == 13) {
	    mock_toc.tracks[i].adr = CUEIFY_SUB_Q_POSITION;
	    mock_toc.tracks[i].control = CUEIFY_TOC_TRACK_IS_DATA;
	} else {
	    mock_toc.tracks[i].adr = CUEIFY_SUB_Q_NOTHING;
	    mock_toc.tracks[i].control = 0;
	}
	mock_toc.tracks[i].lba = lbas[i];
    }
}


void teardown() {
    /* Not needed. */
}


START_TEST (test_serialize)
{
    cueify_toc *toc = (cueify_toc *)&mock_toc;
    size_t size;
    uint8_t buffer[sizeof(serialized_mock_toc) + 10];

    size = sizeof(buffer);

    fail_unless(cueify_toc_serialize(toc, buffer, &size) == CUEIFY_OK,
		"Could not serialize TOC");
    fail_unless(size == sizeof(serialized_mock_toc),
		"Serialized TOC size incorrect");
    fail_unless(memcmp(buffer, serialized_mock_toc,
		       sizeof(serialized_mock_toc)) == 0,
		"Serialized TOC incorrect");
}
END_TEST


START_TEST (test_deserialize)
{
    cueify_toc_private deserialized_mock_toc;
    cueify_toc *toc = (cueify_toc *)&deserialized_mock_toc;

    fail_unless(cueify_toc_deserialize(toc, serialized_mock_toc,
				       sizeof(serialized_mock_toc)) ==
		CUEIFY_OK,
		"Could not deserialize TOC");
    fail_unless(memcmp(&deserialized_mock_toc, &mock_toc,
		       sizeof(mock_toc)) == 0,
		"Deserialized TOC incorrect");
}
END_TEST


START_TEST (test_getters)
{
    cueify_toc *toc = (cueify_toc *)&mock_toc;

    fail_unless(cueify_toc_get_first_track(toc) == 1,
		"First track in TOC did not match");
    fail_unless(cueify_toc_get_last_track(toc) == 13,
		"Last track in TOC did not match");
    fail_unless(cueify_toc_get_track_control_flags(toc, 13) ==
		CUEIFY_TOC_TRACK_IS_DATA,
		"Track control flags did not match");
    fail_unless(cueify_toc_get_track_control_flags(toc,
						   CUEIFY_LEAD_OUT_TRACK) == 0,
		"Track control flags did not match");
    fail_unless(cueify_toc_get_track_sub_q_channel_format(toc, 13) ==
		CUEIFY_SUB_Q_POSITION,
		"Sub Q channel format did not match");
    fail_unless(cueify_toc_get_track_sub_q_channel_format(
		    toc, CUEIFY_LEAD_OUT_TRACK) == CUEIFY_SUB_Q_NOTHING,
		"Sub Q channel format did not match");
    fail_unless(cueify_toc_get_track_address(toc, 13) == 244076,
		"Track address did not match");
    fail_unless(cueify_toc_get_track_address(toc, CUEIFY_LEAD_OUT_TRACK)
		== 258988,
		"Track address did not match");
    fail_unless(cueify_toc_get_disc_length(toc) == 258988,
		"Disc length did not match");
    fail_unless(cueify_toc_get_track_length(toc, 13) == 258988 - 244076,
		"Track length did not match");
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("toc");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_serialize);
    tcase_add_test(tc_core, test_deserialize);
    tcase_add_test(tc_core, test_getters);
    suite_add_tcase(s, tc_core);

    return s;
}


int main() {
    int number_failed;
    Suite *s = toc_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
