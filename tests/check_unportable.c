/* check_unportable.c - Unit tests for unportable libcueify APIs
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
#include <libcueify/sessions.h>
#include <libcueify/full_toc.h>
#include <libcueify/cdtext.h>
#include <libcueify/mcn_isrc.h>
#include <libcueify/data_mode.h>

#include "check_unportable.cdt.h"

/* Create a binary track descriptor from a TOC. */
#define TRACK_DESCRIPTOR(adr, ctrl, track, address) \
    0, (((adr & 0xF) << 4) | (ctrl & 0xF)), track, 0,			\
	(address >> 24), ((address >> 16) & 0xFF),			\
	((address >> 8) & 0xFF), (address & 0xFF)

uint8_t expected_toc[] = {
    ((8 * 14 + 2) >> 8), ((8 * 14 + 2) & 0xFF), 1, 13,
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 1, 0),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 2, 21445),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 3, 34557),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 4, 61903),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 5, 83000),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 6, 98620),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 7, 112124),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 8, 135655),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 9, 154145),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 10, 176766),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 11, 194590),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, 0, 12, 213436),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, CUEIFY_TOC_TRACK_IS_DATA, 13,
		     244076),
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, CUEIFY_TOC_TRACK_IS_DATA, 0xAA,
		     258988)
};


uint8_t expected_sessions[] = {
    (10 >> 8), (10 & 0xFF), 1, 2,
    TRACK_DESCRIPTOR(CUEIFY_SUB_Q_POSITION, CUEIFY_TOC_TRACK_IS_DATA, 13,
		     244076),
};


/* Create a binary track descriptor from a full TOC. */
#define FULL_TOC_TRACK_DESCRIPTOR(session, adr, ctrl, track,		\
				  abs_min, abs_sec, abs_frm, min, sec, frm) \
    session, (((adr & 0xF) << 4) | (ctrl & 0xF)), 0, track,		\
	abs_min, abs_sec, abs_frm, 0, min, sec, frm


uint8_t expected_full_toc[] = {
    (((13 + 2 * 3) * 11 + 2) >> 8), (((13 + 2 * 3) * 11 + 2) & 0xFF), 1, 2,
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 0xA0, 0, 0, 0, 1,
			      CUEIFY_SESSION_MODE_1, 0),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 0xA1, 0, 0, 0, 12, 0, 0),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 0xA2, 0, 0, 0, 51, 44, 26),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 1, 0, 0, 0, 0, 2, 0),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 2, 0, 0, 0, 4, 47, 70),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 3, 0, 0, 0, 7, 42, 57),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 4, 0, 0, 0, 13, 47, 28),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 5, 0, 0, 0, 18, 28, 50),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 6, 0, 0, 0, 21, 56, 70),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 7, 0, 0, 0, 24, 56, 74),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 8, 0, 0, 0, 30, 10, 55),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 9, 0, 0, 0, 34, 17, 20),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 10, 0, 0, 0, 39, 18, 66),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 11, 0, 0, 0, 43, 16, 40),
    FULL_TOC_TRACK_DESCRIPTOR(1, 1, 0, 12, 0, 0, 0, 47, 27, 61),
    FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA0, 0, 0, 0, 13,
			      CUEIFY_SESSION_MODE_2, 0),
    FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA1, 0, 0, 0, 13, 0, 0),
    FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 0xA2, 0, 0, 0, 57, 35, 13),
    FULL_TOC_TRACK_DESCRIPTOR(2, 1, 4, 13, 0, 0, 0, 54, 16, 26)
};


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


START_TEST (test_toc)
{
    cueify_toc *toc;
    size_t size = sizeof(expected_toc);
    uint8_t buffer[sizeof(expected_toc)];

    toc = cueify_toc_new();
    fail_unless(toc != NULL, "Failed to create cueify_toc object");
    fail_unless(cueify_device_read_toc(dev, toc) == CUEIFY_OK,
		"Failed to read TOC from device");
    fail_unless(cueify_toc_serialize(toc, buffer, &size) == CUEIFY_OK,
		"Could not serialize TOC");
    fail_unless(size == sizeof(expected_toc), "TOC size incorrect");
    fail_unless(memcmp(buffer, expected_toc, sizeof(expected_toc)) == 0,
		"TOC incorrect");
    cueify_toc_free(toc);
}
END_TEST


START_TEST (test_sessions)
{
    cueify_sessions *sessions;
    size_t size = sizeof(expected_sessions);
    uint8_t buffer[sizeof(expected_sessions)];

    sessions = cueify_sessions_new();
    fail_unless(sessions != NULL, "Failed to create cueify_sessions object");
    fail_unless(cueify_device_read_sessions(dev, sessions) == CUEIFY_OK,
		"Failed to read multisession data from device");
    fail_unless(cueify_sessions_serialize(sessions, buffer, 
					  &size) == CUEIFY_OK,
		"Could not serialize multisession data");
    fail_unless(size == sizeof(expected_sessions),
		"Multisession data size incorrect");
    fail_unless(memcmp(buffer, expected_sessions,
		       sizeof(expected_sessions)) == 0,
		"Multisession data incorrect");
    cueify_sessions_free(sessions);
}
END_TEST


START_TEST (test_full_toc)
{
    cueify_full_toc *full_toc;
    size_t size = sizeof(expected_full_toc);
    uint8_t buffer[sizeof(expected_full_toc)];

    full_toc = cueify_full_toc_new();
    fail_unless(full_toc != NULL, "Failed to create cueify_full_toc object");
    fail_unless(cueify_device_read_full_toc(dev, full_toc) == CUEIFY_OK,
		"Failed to read full TOC from device");
    fail_unless(cueify_full_toc_serialize(full_toc, buffer, 
					  &size) == CUEIFY_OK,
		"Could not serialize full TOC");
    fail_unless(size == sizeof(expected_full_toc),
		"Full TOC size incorrect");
    fail_unless(memcmp(buffer, expected_full_toc,
		       sizeof(expected_full_toc)) == 0,
		"Full TOC incorrect");
    cueify_full_toc_free(full_toc);
}
END_TEST


START_TEST (test_cdtext)
{
    cueify_cdtext *cdtext;
    size_t size = sizeof(expected_cdtext);
    uint8_t buffer[sizeof(expected_cdtext)];

    cdtext = cueify_cdtext_new();
    fail_unless(cdtext != NULL, "Failed to create cueify_cdtext object");
    fail_unless(cueify_device_read_cdtext(dev, cdtext) == CUEIFY_OK,
		"Failed to read CD-Text from device");
    fail_unless(cueify_cdtext_serialize(cdtext, buffer, &size) == CUEIFY_OK,
		"Could not serialize CD-Text");
    fail_unless(size == sizeof(expected_cdtext),
		"CD-Text size incorrect");
    fail_unless(memcmp(buffer, expected_cdtext, sizeof(expected_cdtext)) == 0,
		"CD-Text incorrect");
    cueify_cdtext_free(cdtext);
}
END_TEST


START_TEST (test_mcn_isrc)
{
    char *isrcs[] = {
	"USRF30200001", "USRF30200002", "USRF30200003", "USRF30200004",
	"USRF30200005", "USRF30200006", "USRF30200007", "USRF30200008",
	"USRF30200009", "USRF30200010", "USRF30200011", "USRF30200012"
    };
    char buffer[32];  /* This is a reasonable buffer size */
    size_t size = sizeof(buffer);
    int i;

    fail_unless(cueify_device_read_mcn(dev, buffer, &size) == CUEIFY_NO_DATA,
		"Failed to (not) read media catalog number from device");
    fail_unless(size == 1, "Media catalog number size incorrect");
    fail_unless(strcmp(buffer, "") == 0, "Media catalog number not empty");

    for (i = 0; i < 12; i++) {
	size = sizeof(buffer);
	fail_unless(cueify_device_read_isrc(dev, i + 1, buffer,
					    &size) == CUEIFY_OK,
		    "Failed to read ISRC from device");
	/* Some OSs will only fill the length of the ISRC string */
	fail_unless(size == 16 || size == 13, "ISRC size incorrect");
	fail_unless(strcmp(buffer, isrcs[i]) == 0, "ISRC not correct");
    }
}
END_TEST


START_TEST (test_data_mode)
{
    int data_modes[] = {
	CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA,
	CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA,
	CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA,
	CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA, CUEIFY_DATA_MODE_CDDA,
	CUEIFY_DATA_MODE_MODE_2
    };
    int i;

    for (i = 0; i < 13; i++) {
	fail_unless(cueify_device_read_data_mode(dev, i + 1) == data_modes[i],
		    "Failed to read data mode from device");
    }
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("unportable");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_toc);
    tcase_add_test(tc_core, test_sessions);
    tcase_add_test(tc_core, test_full_toc);
    tcase_add_test(tc_core, test_cdtext);
    suite_add_tcase(s, tc_core);

    /* Extra test-case for seek-based tests, which are slower. */
    TCase *tc_seekbased = tcase_create("seekbased");
    tcase_set_timeout(tc_seekbased, 10);
    tcase_add_checked_fixture(tc_seekbased, setup, teardown);
    tcase_add_test(tc_seekbased, test_mcn_isrc);
    tcase_add_test(tc_seekbased, test_data_mode);
    suite_add_tcase(s, tc_seekbased);

    return s;
}


int main() {
    int number_failed;
    Suite *s = toc_suite();
    SRunner *sr = srunner_create(s);

    printf("NOTE: These tests are expected to fail except when (certain\n"
	   "      printings of) David Bowie's Heathen is present in the\n"
	   "      current computer's CD drive.\n\n");

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
