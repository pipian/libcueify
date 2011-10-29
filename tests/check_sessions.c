/* check_sessions.h - Unit tests for libcueify multi-session APIs
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
#include <libcueify/sessions.h>
#include "sessions_private.h"

/* Create a binary track descriptor from a TOC. */
#define TRACK_DESCRIPTOR(adr, ctrl, track, address) \
    0, (((adr & 0xF) << 4) | (ctrl & 0xF)), track, 0,			\
	(address >> 24), ((address >> 16) & 0xFF),			\
	((address >> 8) & 0xFF), (address & 0xFF)

uint8_t serialized_mock_sessions[] = {
    (10 >> 8), (10 & 0xFF), 1, 2,
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, CUEIFY_TOC_TRACK_IS_DATA, 13,
		     244076),
};

cueify_sessions_private mock_sessions;

void setup() {
    memset(&mock_sessions, 0, sizeof(mock_sessions));

    mock_sessions.first_session_number = 1;
    mock_sessions.last_session_number = 2;
    mock_sessions.track_adr = CUEIFY_SUB_Q_POSITION;
    mock_sessions.track_control = CUEIFY_TOC_TRACK_IS_DATA;
    mock_sessions.track_number = 13;
    mock_sessions.track_lba = 244076;
}


void teardown() {
    /* Not needed. */
}


START_TEST (test_serialize)
{
    cueify_sessions *sessions = (cueify_sessions *)&mock_sessions;
    size_t size;
    uint8_t buffer[sizeof(serialized_mock_sessions) + 10];

    size = sizeof(buffer);

    fail_unless(cueify_sessions_serialize(sessions, buffer, &size) ==
		CUEIFY_OK,
		"Could not serialize multisession data");
    fail_unless(size == sizeof(serialized_mock_sessions),
		"Serialized multisession data size incorrect");
    fail_unless(memcmp(buffer, serialized_mock_sessions,
		       sizeof(serialized_mock_sessions)) == 0,
		"Serialized multisession data incorrect");
}
END_TEST


START_TEST (test_deserialize)
{
    cueify_sessions_private deserialized_mock_sessions;
    cueify_sessions *sessions = (cueify_sessions *)&deserialized_mock_sessions;

    fail_unless(cueify_sessions_deserialize(sessions, serialized_mock_sessions,
				       sizeof(serialized_mock_sessions)) ==
		CUEIFY_OK,
		"Could not deserialize multisession data");
    fail_unless(memcmp(&deserialized_mock_sessions, &mock_sessions,
		       sizeof(mock_sessions)) == 0,
		"Deserialized multisession data incorrect");
}
END_TEST


START_TEST (test_getters)
{
    cueify_sessions *sessions = (cueify_sessions *)&mock_sessions;

    fail_unless(cueify_sessions_get_first_session(sessions) == 1,
		"First session in multisession data did not match");
    fail_unless(cueify_sessions_get_last_session(sessions) == 2,
		"Last session in multisession data did not match");
    fail_unless(cueify_sessions_get_last_session_control_flags(sessions) ==
		CUEIFY_TOC_TRACK_IS_DATA,
		"Track control flags did not match");
    fail_unless(cueify_sessions_get_last_session_sub_q_channel_format(
		    sessions) == CUEIFY_SUB_Q_POSITION,
		"Sub Q channel format did not match");
    fail_unless(cueify_sessions_get_last_session_track_number(sessions) == 13,
		"Last session track number did not match");
    fail_unless(cueify_sessions_get_last_session_address(sessions) == 244076,
		"Track address did not match");
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("sessions");
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
