/* check_full_toc.h - Unit tests for libcueify full TOC APIs
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
#include <libcueify/constants.h>
#include <libcueify/error.h>
#include <libcueify/full_toc.h>
#include "full_toc_private.h"

/* Create a binary track descriptor from a full TOC. */
#define TRACK_DESCRIPTOR(session, adr, ctrl, track,			\
			 abs_min, abs_sec, abs_frm, min, sec, frm)	\
    session, (((adr & 0xF) << 4) | (ctrl & 0xF)), 0, track,		\
	abs_min, abs_sec, abs_frm, 0, min, sec, frm

uint8_t serialized_mock_full_toc[] = {
    (((13 + 2 * 3) * 11 + 2) >> 8), (((13 + 2 * 3) * 11 + 2) & 0xFF), 1, 2,
    TRACK_DESCRIPTOR(1, 1, 4, 0xA0, 0, 0, 0, 1, CUEIFY_DISC_MODE_1, 0),
    TRACK_DESCRIPTOR(1, 1, 4, 0xA1, 0, 0, 0, 12, 0, 0),
    TRACK_DESCRIPTOR(1, 1, 4, 0xA2, 0, 0, 0, 51, 44, 26),
    TRACK_DESCRIPTOR(1, 1, 4, 1, 0, 0, 0, 0, 2, 0),
    TRACK_DESCRIPTOR(1, 1, 4, 2, 0, 0, 0, 4, 47, 70),
    TRACK_DESCRIPTOR(1, 1, 4, 3, 0, 0, 0, 7, 42, 57),
    TRACK_DESCRIPTOR(1, 1, 4, 4, 0, 0, 0, 13, 47, 28),
    TRACK_DESCRIPTOR(1, 1, 4, 5, 0, 0, 0, 18, 28, 50),
    TRACK_DESCRIPTOR(1, 1, 4, 6, 0, 0, 0, 21, 56, 70),
    TRACK_DESCRIPTOR(1, 1, 4, 7, 0, 0, 0, 24, 56, 74),
    TRACK_DESCRIPTOR(1, 1, 4, 8, 0, 0, 0, 30, 10, 55),
    TRACK_DESCRIPTOR(1, 1, 4, 9, 0, 0, 0, 34, 17, 20),
    TRACK_DESCRIPTOR(1, 1, 4, 10, 0, 0, 0, 39, 18, 66),
    TRACK_DESCRIPTOR(1, 1, 4, 11, 0, 0, 0, 43, 16, 40),
    TRACK_DESCRIPTOR(1, 1, 4, 12, 0, 0, 0, 47, 27, 61),
    TRACK_DESCRIPTOR(2, 1, 6, 0xA0, 0, 0, 0, 13, 0, 0),
    TRACK_DESCRIPTOR(2, 1, 6, 0xA1, 0, 0, 0, 13, 0, 0),
    TRACK_DESCRIPTOR(2, 1, 6, 0xA2, 0, 0, 0, 57, 35, 13),
    TRACK_DESCRIPTOR(2, 1, 6, 13, 1, 2, 3, 54, 16, 26)
};

cueify_full_toc_private mock_full_toc;

void setup() {
    cueify_msf_t offsets[13] = {
	{0, 2, 0}, {4, 47, 70}, {7, 42, 57}, {13, 47, 28}, {18, 28, 50},
	{21, 56, 70}, {24, 56, 74}, {30, 10, 55}, {34, 17, 20}, {39, 18, 66},
	{43, 16, 40}, {47, 27, 61}, {54, 16, 26}
    };
    int i;

    memset(&mock_full_toc, 0, sizeof(mock_full_toc));

    mock_full_toc.first_session_number = 1;
    mock_full_toc.last_session_number = 2;
    mock_full_toc.first_track_number = 1;
    mock_full_toc.last_track_number = 13;
    for (i = 1; i <= 13; i++) {
	mock_full_toc.tracks[i].session = 1;
	mock_full_toc.tracks[i].adr = 1;
	mock_full_toc.tracks[i].control = 4;
	mock_full_toc.tracks[i].atime.min = 0;
	mock_full_toc.tracks[i].atime.sec = 0;
	mock_full_toc.tracks[i].atime.frm = 0;
	mock_full_toc.tracks[i].offset = offsets[i - 1];
	if (i == 13) {
	    mock_full_toc.tracks[i].session = 2;
	    mock_full_toc.tracks[i].control = 6;
	    mock_full_toc.tracks[i].atime.min = 1;
	    mock_full_toc.tracks[i].atime.sec = 2;
	    mock_full_toc.tracks[i].atime.frm = 3;
	}
    }

    mock_full_toc.sessions[1].first_track_number = 1;
    mock_full_toc.sessions[1].last_track_number = 12;
    mock_full_toc.sessions[1].session_type = CUEIFY_DISC_MODE_1;
    mock_full_toc.sessions[1].leadout = (cueify_msf_t){51, 44, 26};
    for (i = 0; i < 3; i++) {
	mock_full_toc.sessions[1].pseudotracks[i].session = 1;
	mock_full_toc.sessions[1].pseudotracks[i].adr = 1;
	mock_full_toc.sessions[1].pseudotracks[i].control = 4;
	mock_full_toc.sessions[1].pseudotracks[i].atime.min = 0;
	mock_full_toc.sessions[1].pseudotracks[i].atime.sec = 0;
	mock_full_toc.sessions[1].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    mock_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){1, CUEIFY_DISC_MODE_1, 0};
	} else if (i == 1) {
	    mock_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){12, 0, 0};
	} else {
	    mock_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){51, 44, 26};
	}
    }

    mock_full_toc.sessions[2].first_track_number = 13;
    mock_full_toc.sessions[2].last_track_number = 13;
    mock_full_toc.sessions[2].session_type = 0;
    mock_full_toc.sessions[2].leadout = (cueify_msf_t){57, 35, 13};
    for (i = 0; i < 3; i++) {
	mock_full_toc.sessions[2].pseudotracks[i].session = 1;
	mock_full_toc.sessions[2].pseudotracks[i].adr = 1;
	mock_full_toc.sessions[2].pseudotracks[i].control = 6;
	mock_full_toc.sessions[2].pseudotracks[i].atime.min = 0;
	mock_full_toc.sessions[2].pseudotracks[i].atime.sec = 0;
	mock_full_toc.sessions[2].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    mock_full_toc.sessions[2].pseudotracks[i].offset =
		(cueify_msf_t){13, 0, 0};
	} else if (i == 1) {
	    mock_full_toc.sessions[2].pseudotracks[i].offset =
		(cueify_msf_t){13, 0, 0};
	} else {
	    mock_full_toc.sessions[2].pseudotracks[i].offset =
		(cueify_msf_t){57, 35, 13};
	}
    }
}


void teardown() {
    /* Not needed. */
}


START_TEST (test_serialize)
{
    cueify_full_toc *full_toc = (cueify_full_toc *)&mock_full_toc;
    size_t size;
    uint8_t buffer[sizeof(serialized_mock_full_toc) + 10];

    size = sizeof(buffer);

    fail_unless(cueify_full_toc_serialize(full_toc, buffer, &size) ==
		CUEIFY_OK,
		"Could not serialize full TOC");
    fail_unless(size == sizeof(serialized_mock_full_toc),
		"Serialized full TOC size incorrect");
    fail_unless(memcmp(buffer, serialized_mock_full_toc,
		       sizeof(serialized_mock_full_toc)) == 0,
		"Serialized full TOC incorrect");
}
END_TEST


START_TEST (test_deserialize)
{
    cueify_full_toc_private deserialized_mock_full_toc;
    cueify_full_toc *full_toc = (cueify_full_toc *)&deserialized_mock_full_toc;

    fail_unless(cueify_full_toc_deserialize(full_toc, serialized_mock_full_toc,
				       sizeof(serialized_mock_full_toc)) ==
		CUEIFY_OK,
		"Could not deserialize full TOC");
    fail_unless(memcmp(&deserialized_mock_full_toc, &mock_full_toc,
		       sizeof(mock_full_toc)) == 0,
		"Deserialized full TOC incorrect");
}
END_TEST


START_TEST (test_getters)
{
    cueify_full_toc *full_toc = (cueify_full_toc *)&mock_full_toc;

    fail_unless(cueify_full_toc_get_first_session(full_toc) == 1,
		"First session in full TOC did not match");
    fail_unless(cueify_full_toc_get_last_session(full_toc) == 2,
		"Last session in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_session(full_toc, 1) == 1,
		"Session of track in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_session(full_toc, 13) == 2,
		"Session of track in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_control_flags(full_toc, 1) == 4,
		"Track control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_control_flags(full_toc, 13) == 6,
		"Track control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_sub_q_channel_format(full_toc,1)==1,
		"Sub Q channel format in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_sub_q_channel_format(full_toc,13)==1,
		"Sub Q channel format in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_control_flags(
		    full_toc, 1, CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK) == 4,
		"Session control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_control_flags(
		    full_toc, 1, CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK) == 4,
		"Session control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_control_flags(
		    full_toc, 1, CUEIFY_LEAD_OUT_TRACK) == 4,
		"Session control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_control_flags(
		    full_toc, 2, CUEIFY_LEAD_OUT_TRACK) == 6,
		"Session control flags in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_sub_q_channel_format(
		    full_toc, 1, CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK) == 1,
		"Sub Q channel format for point in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_sub_q_channel_format(
		    full_toc, 1, CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK) == 1,
		"Sub Q channel format for point in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_sub_q_channel_format(
		    full_toc, 1, CUEIFY_LEAD_OUT_TRACK) == 1,
		"Sub Q channel format for point in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_sub_q_channel_format(
		    full_toc, 2, CUEIFY_LEAD_OUT_TRACK) == 1,
		"Sub Q channel format for point in full TOC did not match");
    fail_unless(cueify_full_toc_get_point_address(full_toc, 1, 1).min == 0 &&
		cueify_full_toc_get_point_address(full_toc, 1, 1).sec == 0 &&
		cueify_full_toc_get_point_address(full_toc, 1, 1).frm == 0,
		"Point address in full TOC did not match");
    fail_unless(cueify_full_toc_get_point_address(
		    full_toc, 2, CUEIFY_LEAD_OUT_TRACK).min == 0 &&
		cueify_full_toc_get_point_address(
		    full_toc, 2, CUEIFY_LEAD_OUT_TRACK).sec == 0 &&
		cueify_full_toc_get_point_address(
		    full_toc, 2, CUEIFY_LEAD_OUT_TRACK).frm == 0,
		"Point address in full TOC did not match");
    fail_unless(cueify_full_toc_get_point_address(full_toc, 2, 13).min == 1 &&
		cueify_full_toc_get_point_address(full_toc, 2, 13).sec == 2 &&
		cueify_full_toc_get_point_address(full_toc, 2, 13).frm == 3,
		"Point address in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_address(full_toc, 1).min == 0 &&
		cueify_full_toc_get_track_address(full_toc, 1).sec == 2 &&
		cueify_full_toc_get_track_address(full_toc, 1).frm == 0,
		"Track address in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_address(full_toc, 13).min == 54 &&
		cueify_full_toc_get_track_address(full_toc, 13).sec == 16 &&
		cueify_full_toc_get_track_address(full_toc, 13).frm == 26,
		"Track address in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_first_track(full_toc, 1) == 1,
		"First track in session in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_first_track(full_toc, 2) == 13,
		"First track in session in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_last_track(full_toc, 1) == 12,
		"Last track in session in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_last_track(full_toc, 2) == 13,
		"Last track in session in full TOC did not match");
    fail_unless(cueify_full_toc_get_first_track(full_toc) == 1,
		"First track in full TOC did not match");
    fail_unless(cueify_full_toc_get_last_track(full_toc) == 13,
		"Last track in full TOC did not match");
    fail_unless(cueify_full_toc_get_disc_type(full_toc) == CUEIFY_DISC_MODE_1,
		"Disc type in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_leadout_address(
		    full_toc, 2).min == 57 &&
		cueify_full_toc_get_session_leadout_address(
		    full_toc, 2).sec == 35 &&
		cueify_full_toc_get_session_leadout_address(
		    full_toc, 2).frm == 13,
		"Session lead-out address in full TOC did not match");
    fail_unless(cueify_full_toc_get_disc_length(full_toc).min == 57 &&
		cueify_full_toc_get_disc_length(full_toc).sec == 35 &&
		cueify_full_toc_get_disc_length(full_toc).frm == 13,
		"Disc length in full TOC did not match");
    fail_unless(cueify_full_toc_get_track_length(full_toc, 12).min == 4 &&
		cueify_full_toc_get_track_length(full_toc, 12).sec == 16 &&
		cueify_full_toc_get_track_length(full_toc, 12).frm == 40,
		"Track length in full TOC did not match");
    fail_unless(cueify_full_toc_get_session_length(full_toc, 2).min == 3 &&
		cueify_full_toc_get_session_length(full_toc, 2).sec == 18 &&
		cueify_full_toc_get_session_length(full_toc, 2).frm == 62,
		"Session length in full TOC did not match");
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("full_toc");
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
