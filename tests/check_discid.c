/* check_discid.h - Unit tests for libcueify discid APIs
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
#include <cueify/types.h>
#include <cueify/error.h>
#include <cueify/toc.h>
#include <cueify/sessions.h>
#include <cueify/full_toc.h>
#include <cueify/discid.h>
#include "toc_private.h"
#include "sessions_private.h"
#include "full_toc_private.h"

cueify_toc_private cdda_toc, data_first_toc, data_last_toc;
cueify_sessions_private cdda_sessions, data_first_sessions, data_last_sessions;
cueify_full_toc_private cdda_full_toc, data_first_full_toc, data_last_full_toc;

static inline void lba_to_msf(uint32_t lba, cueify_msf_t *msf) {
    msf->min = lba / 75 / 60;
    msf->sec = lba / 75 % 60;
    msf->frm = lba % 75;

    /* It really should be the MSF value. */
    msf->sec += 2;
    if (msf->sec > 59) {
	msf->min++;
	msf->sec -= 60;
    }
}  /* lba_to_msf */


void setup() {
    uint32_t cdda_lbas[14] = {
	179318, 33, 9215, 21515, 37148, 49738, 61485, 77613, 89395, 112275,
	124103, 136260, 148358, 154275
    };
    uint32_t data_first_lbas[11] = {
	262878, 0, 183381, 186504, 190348, 198331, 208208, 217926, 230854,
	243284, 251868
    };
    uint32_t data_last_lbas[14] = {
	258988, 0, 21445, 34557, 61903, 83000, 98620, 112124, 135655, 154145,
	176766, 194590, 213436, 244076
    };
    int i;

    memset(&cdda_toc, 0, sizeof(cdda_toc));
    memset(&data_first_toc, 0, sizeof(data_first_toc));
    memset(&data_last_toc, 0, sizeof(data_last_toc));
    memset(&cdda_sessions, 0, sizeof(cdda_sessions));
    memset(&data_first_sessions, 0, sizeof(data_first_sessions));
    memset(&data_last_sessions, 0, sizeof(data_last_sessions));
    memset(&cdda_full_toc, 0, sizeof(cdda_full_toc));
    memset(&data_first_full_toc, 0, sizeof(data_first_full_toc));
    memset(&data_last_full_toc, 0, sizeof(data_last_full_toc));

    /* cdda_toc */
    cdda_toc.first_track_number = 1;
    cdda_toc.last_track_number = 13;
    for (i = 0; i < 14; i++) {
	cdda_toc.tracks[i].adr = CUEIFY_SUB_Q_NOTHING;
	cdda_toc.tracks[i].control = 0;
	cdda_toc.tracks[i].lba = cdda_lbas[i];
    }

    /* cdda_sessions */
    cdda_sessions.first_session_number = 1;
    cdda_sessions.last_session_number = 1;
    cdda_sessions.track_adr = CUEIFY_SUB_Q_NOTHING;
    cdda_sessions.track_control = 0;
    cdda_sessions.track_number = 1;
    cdda_sessions.track_lba = 33;

    /* cdda_full_toc */
    cdda_full_toc.first_session_number = 1;
    cdda_full_toc.last_session_number = 1;
    cdda_full_toc.first_track_number = 1;
    cdda_full_toc.last_track_number = 13;
    for (i = 1; i <= 13; i++) {
	cdda_full_toc.tracks[i].session = 1;
	cdda_full_toc.tracks[i].adr = 1;
	cdda_full_toc.tracks[i].control = 4;
	cdda_full_toc.tracks[i].atime.min = 0;
	cdda_full_toc.tracks[i].atime.sec = 0;
	cdda_full_toc.tracks[i].atime.frm = 0;
	lba_to_msf(cdda_lbas[i], &cdda_full_toc.tracks[i].offset);
    }
    cdda_full_toc.sessions[1].first_track_number = 1;
    cdda_full_toc.sessions[1].last_track_number = 13;
    cdda_full_toc.sessions[1].session_type = CUEIFY_SESSION_MODE_1;
    lba_to_msf(cdda_lbas[0], &cdda_full_toc.sessions[1].leadout);
    for (i = 0; i < 3; i++) {
	cdda_full_toc.sessions[1].pseudotracks[i].session = 1;
	cdda_full_toc.sessions[1].pseudotracks[i].adr = 1;
	cdda_full_toc.sessions[1].pseudotracks[i].control = 4;
	cdda_full_toc.sessions[1].pseudotracks[i].atime.min = 0;
	cdda_full_toc.sessions[1].pseudotracks[i].atime.sec = 0;
	cdda_full_toc.sessions[1].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    lba_to_msf(
		cdda_lbas[0],
		&cdda_full_toc.sessions[1].pseudotracks[i].offset);
	} else if (i == 1) {
	    cdda_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){1, CUEIFY_SESSION_MODE_1, 0};
	} else {
	    cdda_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){10, 0, 0};
	}
    }

    /* data_first_toc */
    data_first_toc.first_track_number = 1;
    data_first_toc.last_track_number = 10;
    for (i = 0; i < 11; i++) {
	if (i == 1) {
	    data_first_toc.tracks[i].control = CUEIFY_TOC_TRACK_IS_DATA;
	} else {
	    data_first_toc.tracks[i].control = 0;
	}
	data_first_toc.tracks[i].adr = CUEIFY_SUB_Q_NOTHING;
	data_first_toc.tracks[i].lba = data_first_lbas[i];
    }

    /* data_first_sessions */
    data_first_sessions.first_session_number = 1;
    data_first_sessions.last_session_number = 1;
    data_first_sessions.track_adr = CUEIFY_SUB_Q_NOTHING;
    data_first_sessions.track_control = CUEIFY_TOC_TRACK_IS_DATA;
    data_first_sessions.track_number = 1;
    data_first_sessions.track_lba = 0;

    /* data_first_full_toc */
    data_first_full_toc.first_session_number = 1;
    data_first_full_toc.last_session_number = 1;
    data_first_full_toc.first_track_number = 1;
    data_first_full_toc.last_track_number = 10;
    for (i = 1; i <= 10; i++) {
	data_first_full_toc.tracks[i].session = 1;
	data_first_full_toc.tracks[i].adr = 1;
	data_first_full_toc.tracks[i].control = 4;
	data_first_full_toc.tracks[i].atime.min = 0;
	data_first_full_toc.tracks[i].atime.sec = 0;
	data_first_full_toc.tracks[i].atime.frm = 0;
	lba_to_msf(data_first_lbas[i], &data_first_full_toc.tracks[i].offset);
	if (i == 1) {
	    data_first_full_toc.tracks[i].control = 6;
	}
    }
    data_first_full_toc.sessions[1].first_track_number = 1;
    data_first_full_toc.sessions[1].last_track_number = 10;
    data_first_full_toc.sessions[1].session_type = CUEIFY_SESSION_MODE_1;
    lba_to_msf(data_first_lbas[0], &data_first_full_toc.sessions[1].leadout);
    for (i = 0; i < 3; i++) {
	data_first_full_toc.sessions[1].pseudotracks[i].session = 1;
	data_first_full_toc.sessions[1].pseudotracks[i].adr = 1;
	data_first_full_toc.sessions[1].pseudotracks[i].control = 4;
	data_first_full_toc.sessions[1].pseudotracks[i].atime.min = 0;
	data_first_full_toc.sessions[1].pseudotracks[i].atime.sec = 0;
	data_first_full_toc.sessions[1].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    lba_to_msf(
		data_first_lbas[0],
		&data_first_full_toc.sessions[1].pseudotracks[i].offset);
	} else if (i == 1) {
	    data_first_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){1, CUEIFY_SESSION_MODE_1, 0};
	} else {
	    data_first_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){10, 0, 0};
	}
    }

    /* data_last_toc */
    data_last_toc.first_track_number = 1;
    data_last_toc.last_track_number = 13;
    for (i = 0; i < 14; i++) {
	if (i == 13) {
	    data_last_toc.tracks[i].control = CUEIFY_TOC_TRACK_IS_DATA;
	} else {
	    data_last_toc.tracks[i].control = 0;
	}
	data_last_toc.tracks[i].adr = CUEIFY_SUB_Q_NOTHING;
	data_last_toc.tracks[i].lba = data_last_lbas[i];
    }

    /* data_last_sessions */
    data_last_sessions.first_session_number = 1;
    data_last_sessions.last_session_number = 2;
    data_last_sessions.track_adr = CUEIFY_SUB_Q_NOTHING;
    data_last_sessions.track_control = CUEIFY_TOC_TRACK_IS_DATA;
    data_last_sessions.track_number = 13;
    data_last_sessions.track_lba = 244076;

    /* data_last_full_toc */
    data_last_full_toc.first_session_number = 1;
    data_last_full_toc.last_session_number = 2;
    data_last_full_toc.first_track_number = 1;
    data_last_full_toc.last_track_number = 13;
    for (i = 1; i <= 13; i++) {
	data_last_full_toc.tracks[i].session = 1;
	data_last_full_toc.tracks[i].adr = 1;
	data_last_full_toc.tracks[i].control = 4;
	data_last_full_toc.tracks[i].atime.min = 0;
	data_last_full_toc.tracks[i].atime.sec = 0;
	data_last_full_toc.tracks[i].atime.frm = 0;
	lba_to_msf(data_last_lbas[i], &data_last_full_toc.tracks[i].offset);
	if (i == 13) {
	    data_last_full_toc.tracks[i].session = 2;
	    data_last_full_toc.tracks[i].control = 6;
	}
    }
    data_last_full_toc.sessions[1].first_track_number = 1;
    data_last_full_toc.sessions[1].last_track_number = 12;
    data_last_full_toc.sessions[1].session_type = CUEIFY_SESSION_MODE_1;
    data_last_full_toc.sessions[1].leadout = (cueify_msf_t){51, 44, 26};
    for (i = 0; i < 3; i++) {
	data_last_full_toc.sessions[1].pseudotracks[i].session = 1;
	data_last_full_toc.sessions[1].pseudotracks[i].adr = 1;
	data_last_full_toc.sessions[1].pseudotracks[i].control = 4;
	data_last_full_toc.sessions[1].pseudotracks[i].atime.min = 0;
	data_last_full_toc.sessions[1].pseudotracks[i].atime.sec = 0;
	data_last_full_toc.sessions[1].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    data_last_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){51, 44, 26};
	} else if (i == 1) {
	    data_last_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){1, CUEIFY_SESSION_MODE_1, 0};
	} else {
	    data_last_full_toc.sessions[1].pseudotracks[i].offset =
		(cueify_msf_t){12, 0, 0};
	}
    }
    data_last_full_toc.sessions[2].first_track_number = 13;
    data_last_full_toc.sessions[2].last_track_number = 13;
    data_last_full_toc.sessions[2].session_type = CUEIFY_SESSION_MODE_2;
    lba_to_msf(data_last_lbas[0], &data_last_full_toc.sessions[2].leadout);
    for (i = 0; i < 3; i++) {
	data_last_full_toc.sessions[2].pseudotracks[i].session = 2;
	data_last_full_toc.sessions[2].pseudotracks[i].adr = 1;
	data_last_full_toc.sessions[2].pseudotracks[i].control = 6;
	data_last_full_toc.sessions[2].pseudotracks[i].atime.min = 0;
	data_last_full_toc.sessions[2].pseudotracks[i].atime.sec = 0;
	data_last_full_toc.sessions[2].pseudotracks[i].atime.frm = 0;
	if (i == 0) {
	    lba_to_msf(data_last_lbas[0],
		       &data_last_full_toc.sessions[2].pseudotracks[i].offset);
	} else if (i == 1) {
	    data_last_full_toc.sessions[2].pseudotracks[i].offset =
		(cueify_msf_t){13, CUEIFY_SESSION_MODE_2, 0};
	} else {
	    data_last_full_toc.sessions[2].pseudotracks[i].offset =
		(cueify_msf_t){13, 0, 0};
	}
    }
}


void teardown() {
    /* Not needed. */
}

#define CDDA_FREEDB_ID                 0xc009560d
#define DATA_FIRST_FREEDB_ID           0x9d0db10a
#define DATA_LAST_FREEDB_ID            0xbe0d7d0d
#define DATA_LAST_LIBDISCID_FREEDB_ID  0xae0c1e0c

START_TEST (test_toc_freedb_cdda)
{
    cueify_toc *toc = (cueify_toc *)&cdda_toc;
    cueify_sessions *sessions = (cueify_sessions *)&cdda_sessions;

    fail_unless(cueify_toc_get_freedb_id(toc, NULL) == CDDA_FREEDB_ID,
		"Did not get correct freedb ID from CDDA TOC");
    fail_unless(cueify_toc_get_freedb_id(toc, sessions) == CDDA_FREEDB_ID,
		"Did not get correct freedb ID from CDDA TOC/Sessions");
}
END_TEST


START_TEST (test_toc_freedb_data_first)
{
    cueify_toc *toc = (cueify_toc *)&data_first_toc;
    cueify_sessions *sessions = (cueify_sessions *)&data_first_sessions;

    fail_unless(cueify_toc_get_freedb_id(toc, NULL) == DATA_FIRST_FREEDB_ID,
		"Did not get correct freedb ID from data-first TOC");
    fail_unless(
	cueify_toc_get_freedb_id(toc, sessions) == DATA_FIRST_FREEDB_ID,
	"Did not get correct freedb ID from data-first TOC/Sessions");
}
END_TEST


START_TEST (test_toc_freedb_data_last)
{
    cueify_toc *toc = (cueify_toc *)&data_last_toc;
    cueify_sessions *sessions = (cueify_sessions *)&data_last_sessions;

    fail_unless(cueify_toc_get_freedb_id(toc, NULL) == DATA_LAST_FREEDB_ID,
		"Did not get correct freedb ID from data-last TOC");
    fail_unless(
	cueify_toc_get_freedb_id(toc, sessions) ==
	DATA_LAST_LIBDISCID_FREEDB_ID,
	"Did not get correct freedb ID from data-last TOC/Sessions");
}
END_TEST


START_TEST (test_full_toc_freedb_cdda)
{
    cueify_full_toc *toc = (cueify_full_toc *)&cdda_full_toc;

    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 1) == CDDA_FREEDB_ID,
	"Did not get correct freedb ID from CDDA full TOC w/data");
    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 0) == CDDA_FREEDB_ID,
	"Did not get correct freedb ID from CDDA full TOC w/o data");
}
END_TEST


START_TEST (test_full_toc_freedb_data_first)
{
    cueify_full_toc *toc = (cueify_full_toc *)&data_first_full_toc;

    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 1) == DATA_FIRST_FREEDB_ID,
	"Did not get correct freedb ID from data-first full TOC w/data");
    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 0) == DATA_FIRST_FREEDB_ID,
	"Did not get correct freedb ID from data-first full TOC w/o data");
}
END_TEST


START_TEST (test_full_toc_freedb_data_last)
{
    cueify_full_toc *toc = (cueify_full_toc *)&data_last_full_toc;

    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 1) == DATA_LAST_FREEDB_ID,
	"Did not get correct freedb ID from data-last full TOC w/data");
    fail_unless(
	cueify_full_toc_get_freedb_id(toc, 0) == DATA_LAST_LIBDISCID_FREEDB_ID,
	"Did not get correct freedb ID from data-last full TOC w/o data");
}
END_TEST

#define CDDA_MUSICBRAINZ_ID        "5cVTKwEtQPTiqT4A3ktiYJ14WzQ-"
#define DATA_FIRST_MUSICBRAINZ_ID  "ry6UkUayKgncrSiv06fnFKJCadM-"
#define DATA_LAST_MUSICBRAINZ_ID   "iIqthNFjPeboX2O1GKpqvcQIWDc-"

START_TEST (test_toc_musicbrainz_cdda)
{
    cueify_toc *toc = (cueify_toc *)&cdda_toc;
    cueify_sessions *sessions = (cueify_sessions *)&cdda_sessions;
    char *mbid;

    mbid = cueify_toc_get_musicbrainz_id(toc, NULL);
    fail_unless(strcmp(mbid, CDDA_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from CDDA TOC");
    free(mbid);
    mbid = cueify_toc_get_musicbrainz_id(toc, sessions);
    fail_unless(strcmp(mbid, CDDA_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from CDDA TOC/Sessions");
    free(mbid);
}
END_TEST


START_TEST (test_toc_musicbrainz_data_first)
{
    cueify_toc *toc = (cueify_toc *)&data_first_toc;
    cueify_sessions *sessions = (cueify_sessions *)&data_first_sessions;
    char *mbid;

    mbid = cueify_toc_get_musicbrainz_id(toc, NULL);
    fail_unless(strcmp(mbid, DATA_FIRST_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from data-first TOC");
    free(mbid);
    mbid = cueify_toc_get_musicbrainz_id(toc, sessions);
    fail_unless(
	strcmp(mbid, DATA_FIRST_MUSICBRAINZ_ID) == 0,
	"Did not get correct MusicBrainz ID from data-first TOC/Sessions");
    free(mbid);
}
END_TEST


START_TEST (test_toc_musicbrainz_data_last)
{
    cueify_toc *toc = (cueify_toc *)&data_last_toc;
    cueify_sessions *sessions = (cueify_sessions *)&data_last_sessions;
    char *mbid;

    /* NOTE: Heuristics will work correctly for this disc. */
    mbid = cueify_toc_get_musicbrainz_id(toc, NULL);
    fail_unless(strcmp(mbid, DATA_LAST_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from data-last TOC");
    free(mbid);
    mbid = cueify_toc_get_musicbrainz_id(toc, sessions);
    fail_unless(
	strcmp(mbid, DATA_LAST_MUSICBRAINZ_ID) == 0,
	"Did not get correct MusicBrainz ID from data-last TOC/Sessions");
    free(mbid);
}
END_TEST


START_TEST (test_full_toc_musicbrainz_cdda)
{
    cueify_full_toc *toc = (cueify_full_toc *)&cdda_full_toc;
    char *mbid;

    mbid = cueify_full_toc_get_musicbrainz_id(toc);
    fail_unless(strcmp(mbid, CDDA_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from CDDA full TOC");
    free(mbid);
}
END_TEST


START_TEST (test_full_toc_musicbrainz_data_first)
{
    cueify_full_toc *toc = (cueify_full_toc *)&data_first_full_toc;
    char *mbid;

    mbid = cueify_full_toc_get_musicbrainz_id(toc);
    fail_unless(strcmp(mbid, DATA_FIRST_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from data-first full TOC");
    free(mbid);
}
END_TEST


START_TEST (test_full_toc_musicbrainz_data_last)
{
    cueify_full_toc *toc = (cueify_full_toc *)&data_last_full_toc;
    char *mbid;

    mbid = cueify_full_toc_get_musicbrainz_id(toc);
    fail_unless(strcmp(mbid, DATA_LAST_MUSICBRAINZ_ID) == 0,
		"Did not get correct MusicBrainz ID from data-last full TOC");
    free(mbid);
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("discid");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_toc_freedb_cdda);
    tcase_add_test(tc_core, test_toc_freedb_data_first);
    tcase_add_test(tc_core, test_toc_freedb_data_last);
    tcase_add_test(tc_core, test_full_toc_freedb_cdda);
    tcase_add_test(tc_core, test_full_toc_freedb_data_first);
    tcase_add_test(tc_core, test_full_toc_freedb_data_last);
    tcase_add_test(tc_core, test_toc_musicbrainz_cdda);
    tcase_add_test(tc_core, test_toc_musicbrainz_data_first);
    tcase_add_test(tc_core, test_toc_musicbrainz_data_last);
    tcase_add_test(tc_core, test_full_toc_musicbrainz_cdda);
    tcase_add_test(tc_core, test_full_toc_musicbrainz_data_first);
    tcase_add_test(tc_core, test_full_toc_musicbrainz_data_last);
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
