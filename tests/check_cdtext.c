/* check_cdtext.h - Unit tests for libcueify CD-Text APIs
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
#include <libcueify/cdtext.h>
#include "cdtext_private.h"

#include "check_cdtext.cdt.h"

cueify_cdtext_private mock_cdtext;
cueify_cdtext_toc_track_interval_private mock_cdtext_intervals[2] = {
    {{3, 4, 5}, {3, 4, 6}},
    {{4, 5, 6}, {4, 5, 10}}
};

void setup() {
    cueify_cdtext_block_private *b;
    cueify_msf_t offsets[13] = {
	{0, 2, 0}, {4, 47, 70}, {7, 42, 57}, {13, 47, 28}, {18, 28, 50},
	{21, 56, 70}, {24, 56, 74}, {30, 10, 55}, {34, 17, 20}, {39, 18, 66},
	{43, 16, 40}, {47, 27, 61}, {54, 16, 26}
    };
    int i;

    memset(&mock_cdtext, 0, sizeof(mock_cdtext));

    b = &(mock_cdtext.blocks[0]);
    b->valid = 1;
    b->charset = CUEIFY_CDTEXT_CHARSET_ISO8859_1;
    b->language = CUEIFY_CDTEXT_LANG_ENGLISH;
    b->first_track_number = 1;
    b->last_track_number = 12;
    b->program_cdtext = 1;
    b->program_copyright = 1;
    b->message_copyright = 1;
    b->name_copyright = 1;
    b->title_copyright = 1;
    b->titles[0] = "Heathen";
    b->titles[1] = "Sunday";
    b->titles[2] = "Cactus";
    b->titles[3] = "Slip Away";
    b->titles[4] = "Slow Burn";
    b->titles[5] = "Afraid";
    b->titles[6] = "I've Been Waiting For You";
    b->titles[7] = "I Would Be Your Slave";
    b->titles[8] = "I Took A Trip On A Gemini Spaceship";
    b->titles[9] = "5:15 The Angels Have Gone";
    b->titles[10] = "Everyone Says 'Hi'";
    b->titles[11] = "A Better Future";
    b->titles[12] = "Heathen (The Rays)";
    b->performers[0] = "David Bowie";
    b->performers[1] = b->performers[2] = b->performers[3] = b->performers[4] =
	b->performers[5] = b->performers[6] = b->performers[7] =
	b->performers[8] = b->performers[9] = b->performers[10] =
	b->performers[11] = b->performers[12] = "\t";
    b->songwriters[0] = "ASongwriter";
    b->songwriters[1] = "TSongwriter";
    b->songwriters[2] = b->songwriters[3] = b->songwriters[4] =
	b->songwriters[5] = b->songwriters[6] = b->songwriters[7] =
	b->songwriters[8] = b->songwriters[9] = b->songwriters[10] =
	b->songwriters[11] = b->songwriters[12] = "\t";
    b->composers[0] = "AlbComposer";
    b->composers[1] = "TrkComposer";
    b->composers[2] = b->composers[3] = b->composers[4] = b->composers[5] =
	b->composers[6] = b->composers[7] = "\t";
    b->composers[8] = "Tk2Composer";
    b->composers[9] = b->composers[10] = b->composers[11] = b->composers[12] =
	"\t";
    b->arrangers[0] = "AlbArranger";
    b->arrangers[1] = "TrkArranger";
    b->arrangers[2] = b->arrangers[3] = b->arrangers[4] = b->arrangers[5] =
	b->arrangers[6] = b->arrangers[7] = b->arrangers[8] = b->arrangers[9] =
	b->arrangers[10] = b->arrangers[11] = b->arrangers[12] = "\t";
    b->messages[0] = "AlbMessages";
    b->messages[1] = "TrkMessages";
    b->messages[2] = b->messages[3] = b->messages[4] = b->messages[5] =
	b->messages[6] = b->messages[7] = b->messages[8] = b->messages[9] =
	b->messages[10] = b->messages[11] = b->messages[12] = "\t";
    b->private[0] = "OnlyPrivate";
    b->messages[1] = b->messages[2] = b->messages[3] = b->messages[4] =
	b->messages[5] = b->messages[6] = b->messages[7] = b->messages[8] =
	b->messages[9] = b->messages[10] = b->messages[11] =
	b->messages[12] = "";
    b->upc_isrcs[0] = "";
    b->upc_isrcs[1] = "US-RF3-02-00001";
    b->upc_isrcs[2] = "US-RF3-02-00002";
    b->upc_isrcs[3] = "US-RF3-02-00003";
    b->upc_isrcs[4] = "US-RF3-02-00004";
    b->upc_isrcs[5] = "US-RF3-02-00005";
    b->upc_isrcs[6] = "US-RF3-02-00006";
    b->upc_isrcs[7] = "US-RF3-02-00007";
    b->upc_isrcs[8] = "US-RF3-02-00008";
    b->upc_isrcs[9] = "US-RF3-02-00009";
    b->upc_isrcs[10] = "US-RF3-02-00010";
    b->upc_isrcs[11] = "US-RF3-02-00011";
    b->upc_isrcs[12] = "US-RF3-02-00012";
    b->discid = "CK86630";
    b->genre_code = 0x20;
    b->genre_name = "Genre #32";

    b = &(mock_cdtext.blocks[1]);
    b->valid = 1;
    b->charset = CUEIFY_CDTEXT_CHARSET_MSJIS;
    b->language = CUEIFY_CDTEXT_LANG_JAPANESE;
    b->first_track_number = 1;
    b->last_track_number = 12;
    b->name_copyright = 1;
    b->title_copyright = 1;
    b->titles[0] = "\xE3\x83\x92\xE3\x83\xBC\xE3\x82\xBC\xE3\x83\xB3";
    b->titles[1] = "\t";
    b->titles[2] = "\xE3\x82\xAB\xE3\x82\xAF\xE3\x82\xBF\xE3\x82\xB9";
    b->titles[3] = b->titles[4] = b->titles[5] = b->titles[6] = b->titles[7] =
	b->titles[8] = b->titles[9] = b->titles[10] = b->titles[11] =
	b->titles[12] = "\t";
    b->performers[0] = "\xE3\x83\x87\xE3\x83\xB4\xE3\x82\xA3\xE3\x83\x83"
	"\xE3\x83\x89\xE3\x83\xBB\xE3\x89\x9C\xE3\x82\xA6\xE3\x82\xA4";
    b->performers[1] = b->performers[2] = b->performers[3] = b->performers[4] =
	b->performers[5] = b->performers[6] = b->performers[7] =
	b->performers[8] = b->performers[9] = b->performers[10] =
	b->performers[11] = b->performers[12] = "\t";

    mock_cdtext.toc.first_track_number = 1;
    mock_cdtext.toc.last_track_number = 13;
    for (i = 0; i < 13; i++) {
	mock_cdtext.toc.offsets[i + 1] = offsets[i];
    }
    mock_cdtext.toc.num_intervals[2] = 2;
    mock_cdtext.toc.intervals[2] = mock_cdtext_intervals;
}


void teardown() {
    /* Not needed. */
}


START_TEST (test_serialize)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;
    size_t size;
    uint8_t buffer[sizeof(serialized_mock_cdtext) + 10];

    size = sizeof(buffer);

    fail_unless(cueify_cdtext_serialize(cdtext, buffer, &size) ==
		CUEIFY_OK,
		"Could not serialize CD-Text");
    fail_unless(size == sizeof(serialized_mock_cdtext),
		"Serialized CD-Text size incorrect");
    fail_unless(memcmp(buffer, serialized_mock_cdtext,
		       sizeof(serialized_mock_cdtext)) == 0,
		"Serialized CD-Text incorrect");
}
END_TEST


START_TEST (test_deserialize)
{
    cueify_cdtext_private deserialized_mock_cdtext;
    cueify_cdtext *cdtext = (cueify_cdtext *)&deserialized_mock_cdtext;

    fail_unless(cueify_cdtext_deserialize(cdtext, serialized_mock_cdtext,
					  sizeof(serialized_mock_cdtext)) ==
		CUEIFY_OK,
		"Could not deserialize CD-Text");
    fail_unless(memcmp(&deserialized_mock_cdtext, &mock_cdtext,
		       sizeof(mock_cdtext)) == 0,
		"Deserialized CD-Text incorrect");
}
END_TEST


START_TEST (test_getters)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;

    fail_unless(cueify_cdtext_get_toc_first_track(cdtext) == 1,
		"First track in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_last_track(cdtext) == 13,
		"Last session in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_track_offset(cdtext, 13).min == 54 &&
		cueify_cdtext_get_toc_track_offset(cdtext, 13).sec == 16 &&
		cueify_cdtext_get_toc_track_offset(cdtext, 13).frm == 26,
		"Offset of track in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_num_track_intervals(cdtext, 1) == 0,
		"Number of intervals of track in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_num_track_intervals(cdtext, 2) == 2,
		"Number of intervals of track in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_track_interval_start(cdtext,
							   2, 0).min == 4 &&
		cueify_cdtext_get_toc_track_interval_start(cdtext,
							   2, 0).sec == 5 &&
		cueify_cdtext_get_toc_track_interval_start(cdtext,
							   2, 0).frm == 6,
		"Interval start in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 0).min == 4 &&
		cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 0).sec == 5 &&
		cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 0).frm == 10,
		"Interval start in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_num_blocks(cdtext) == 2,
		"Number of blocks in CD-Text did not match");
}
END_TEST


START_TEST (test_block_getters)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;
    cueify_cdtext_block *block = cueify_cdtext_get_block(cdtext, 0);

    fail_unless(block != NULL, "CD-Text block was null");
    fail_unless(cueify_cdtext_block_get_first_track(block) == 1,
		"First track in CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_last_track(block) == 12,
		"Last track in CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_charset(block) ==
		CUEIFY_CDTEXT_CHARSET_ISO8859_1,
		"Character set of CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_language(block) ==
		CUEIFY_CDTEXT_LANG_ENGLISH,
		"Language of CD-Text block did not match");
    fail_unless(cueify_cdtext_block_has_program_data(block) == 1,
		"CD-Text block did not have program data");
    fail_unless(cueify_cdtext_block_has_program_copyright(block) == 1,
		"CD-Text block did not have program copyright");
    fail_unless(cueify_cdtext_block_has_message_copyright(block) == 1,
		"CD-Text block did not have message copyright");
    fail_unless(cueify_cdtext_block_has_name_copyright(block) == 1,
		"CD-Text block did not have name copyright");
    fail_unless(cueify_cdtext_block_has_title_copyright(block) == 1,
		"CD-Text block did not have title copyright");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block,
						     CUEIFY_CDTEXT_ALBUM),
		       "Heathen") == 0,
		"Album title in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block, 1),
		       "Sunday") == 0,
		"Track title in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_performer(block,
							 CUEIFY_CDTEXT_ALBUM),
		       "David Bowie") == 0,
		"Album performer in CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_performer(block, 1) == NULL,
		"Track performer in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_songwriter(block,
							  CUEIFY_CDTEXT_ALBUM),
		       "David Bowie") == 0,
		"Album songwriter in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_songwriter(block, 1),
		       "") == 0,
		"Track songwriter in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_composer(block,
							CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album composer in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_composer(block, 1),
		       "") == 0,
		"Track composer in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_arranger(block,
							CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album arranger in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_arranger(block, 1),
		       "") == 0,
		"Track arranger in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_message(block,
						       CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album message in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_message(block, 1),
		       "") == 0,
		"Track message in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_private(block,
						       CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album private data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_private(block, 1),
		       "") == 0,
		"Track private data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_upc_isrc(block,
							CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album UPC data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_upc_isrc(block, 1),
		       "") == 0,
		"Track ISRC data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_discid(block),
		       "") == 0,
		"DiscID in CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_genre_code(block) == 20,
		"Genre code in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_genre_name(block),
		       "") == 0,
		"Genre name in CD-Text block did not match");
}
END_TEST


START_TEST (test_japanese)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;
    cueify_cdtext_block *block = cueify_cdtext_get_block(cdtext, 1);

    fail_unless(block != NULL, "CD-Text block was null");
    fail_unless(cueify_cdtext_block_get_charset(block) ==
		CUEIFY_CDTEXT_CHARSET_MSJIS,
		"Character set of Japanese CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_language(block) ==
		CUEIFY_CDTEXT_LANG_JAPANESE,
		"Language of Japanese CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block,
						     CUEIFY_CDTEXT_ALBUM),
		       "Heathen") == 0,
		"Album title in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block, 1),
		       "") == 0,
		"Track title in CD-Text block did not match");
}
END_TEST


Suite *toc_suite() {
    Suite *s = suite_create("cdtext");
    TCase *tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_serialize);
    tcase_add_test(tc_core, test_deserialize);
    tcase_add_test(tc_core, test_getters);
    tcase_add_test(tc_core, test_block_getters);
    tcase_add_test(tc_core, test_japanese);
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
