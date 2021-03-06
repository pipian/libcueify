/* check_cdtext.h - Unit tests for libcueify CD-Text APIs
 *
 * Copyright (c) 2011, 2012 Ian Jacobi <pipian@pipian.com>
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
#include <cueify/constants.h>
#include <cueify/error.h>
#include <cueify/cdtext.h>
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
    b->charset = CUEIFY_CDTEXT_CHARSET_ASCII;
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
    b->private[1] = b->private[2] = b->private[3] = b->private[4] =
	b->private[5] = b->private[6] = b->private[7] = b->private[8] =
	b->private[9] = b->private[10] = b->private[11] =
	b->private[12] = "";
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
    b->charset = CUEIFY_CDTEXT_CHARSET_ISO8859_1;
    b->language = CUEIFY_CDTEXT_LANG_FRENCH;
    b->first_track_number = 1;
    b->last_track_number = 12;
    b->name_copyright = 1;
    b->title_copyright = 1;
    b->titles[0] = "Pa\xC3""\xAF""en";
    b->titles[1] = "\t";
    b->titles[2] = "\xE2\x96\x88\xE2\x96\x80\xE2\x99\xAA\xE2\x84\x97\xE2\x80\xA0";
    b->titles[3] = "\t";
    b->titles[4] = b->titles[5] = b->titles[6] = b->titles[7] = b->titles[8] =
	b->titles[9] = b->titles[10] = b->titles[11] = b->titles[12] = "";
    b->performers[0] = "David Bowie";
    b->performers[1] = b->performers[2] = b->performers[3] = b->performers[4] =
	b->performers[5] = b->performers[6] = b->performers[7] =
	b->performers[8] = b->performers[9] = b->performers[10] =
	b->performers[11] = b->performers[12] = "\t";

    b = &(mock_cdtext.blocks[2]);
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
    b->titles[3] = "\t";
    b->titles[4] = b->titles[5] = b->titles[6] = b->titles[7] = b->titles[8] =
	b->titles[9] = b->titles[10] = b->titles[11] = b->titles[12] = "";
    b->performers[0] = "\xE3\x83\x87\xE3\x83\xB4\xE3\x82\xA3\xE3\x83\x83"
	"\xE3\x83\x89\xE3\x83\xBB\xE3\x83\x9C\xE3\x82\xA6\xE3\x82\xA4";
    b->performers[1] = b->performers[2] = b->performers[3] = b->performers[4] =
	b->performers[5] = b->performers[6] = b->performers[7] =
	b->performers[8] = b->performers[9] = b->performers[10] = "\t";
    b->performers[11] = "\xF0\x9F\x8E\xA4";
    b->performers[12] = "\xE2\x84\x97\xF3\xA0\x87\xAF";

    mock_cdtext.toc.first_track_number = 1;
    mock_cdtext.toc.last_track_number = 13;
    for (i = 0; i < 13; i++) {
	mock_cdtext.toc.offsets[i + 1] = offsets[i];
    }
    mock_cdtext.toc.offsets[0].min = 57;
    mock_cdtext.toc.offsets[0].sec = 35;
    mock_cdtext.toc.offsets[0].frm = 13;
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
    int i, j;

    memset(&deserialized_mock_cdtext, 0, sizeof(deserialized_mock_cdtext));

    fail_unless(cueify_cdtext_deserialize(cdtext, serialized_mock_cdtext,
					  sizeof(serialized_mock_cdtext)) ==
		CUEIFY_OK,
		"Could not deserialize CD-Text");
    /*
     * Because of pointers, we can no longer do a memcmp between the
     * two objects.
     */
    /* Start by comparing the toc field */
    fail_unless(deserialized_mock_cdtext.toc.first_track_number ==
		mock_cdtext.toc.first_track_number,
		"Deserialized CD-Text TOC first track number incorrect");
    fail_unless(deserialized_mock_cdtext.toc.last_track_number ==
		mock_cdtext.toc.last_track_number,
		"Deserialized CD-Text TOC last track number incorrect");
    for (i = 0; i < MAX_TRACKS; i++) {
	fail_unless(memcmp(&deserialized_mock_cdtext.toc.offsets[i],
			   &mock_cdtext.toc.offsets[i],
			   sizeof(cueify_msf_t)) == 0,
		    "Deserialized CD-Text TOC offsets incorrect");
	fail_unless(deserialized_mock_cdtext.toc.num_intervals[i] ==
		    mock_cdtext.toc.num_intervals[i],
		    "Deserialized CD-Text interval counts incorrect");
	if (deserialized_mock_cdtext.toc.num_intervals[i] ==
	    mock_cdtext.toc.num_intervals[i] &&
	    deserialized_mock_cdtext.toc.num_intervals[i] != 0) {
	    for (j = 0;
		 j < deserialized_mock_cdtext.toc.num_intervals[i];
		 j++) {
		fail_unless(
		    memcmp(
			&deserialized_mock_cdtext.toc.intervals[i][j],
			&mock_cdtext.toc.intervals[i][j],
			sizeof(cueify_cdtext_toc_track_interval_private)) == 0,
		    "Deserialized CD-Text intervals incorrect");
	    }
	} else if (deserialized_mock_cdtext.toc.num_intervals[i] ==
		   mock_cdtext.toc.num_intervals[i]) {
	    fail_unless(deserialized_mock_cdtext.toc.intervals[i] == NULL,
			"Deserialized CD-Text intervals incorrect");
	}
    }

    /* Now test the blocks. */
    for (i = 0; i < MAX_BLOCKS; i++) {
	cueify_cdtext_block_private *deserialized_block =
	    &deserialized_mock_cdtext.blocks[i];
	cueify_cdtext_block_private *mock_block =
	    &mock_cdtext.blocks[i];

	fail_unless(deserialized_block->valid == mock_block->valid,
		    "Deserialized CD-Text block validities incorrect");
	fail_unless(deserialized_block->charset == mock_block->charset,
		    "Deserialized CD-Text block character sets incorrect");
	fail_unless(deserialized_block->language == mock_block->language,
		    "Deserialized CD-Text block languages incorrect");
	fail_unless(deserialized_block->first_track_number ==
		    mock_block->first_track_number,
		    "Deserialized CD-Text block first track number incorrect");
	fail_unless(deserialized_block->last_track_number ==
		    mock_block->last_track_number,
		    "Deserialized CD-Text block last track number incorrect");
	fail_unless(deserialized_block->program_cdtext ==
		    mock_block->program_cdtext,
		    "Deserialized CD-Text block program CD-Text incorrect");
	fail_unless(deserialized_block->program_copyright ==
		    mock_block->program_copyright,
		    "Deserialized CD-Text block program copyright incorrect");
	fail_unless(deserialized_block->message_copyright ==
		    mock_block->message_copyright,
		    "Deserialized CD-Text block message copyright incorrect");
	fail_unless(deserialized_block->name_copyright ==
		    mock_block->name_copyright,
		    "Deserialized CD-Text block name copyright incorrect");
	fail_unless(deserialized_block->title_copyright ==
		    mock_block->title_copyright,
		    "Deserialized CD-Text block title copyright incorrect");

	for (j = 0; j < MAX_TRACKS; j++) {
	    fail_unless((deserialized_block->titles[i] == NULL &&
			 mock_block->titles[i] == NULL) ||
			strcmp(deserialized_block->titles[i],
			       mock_block->titles[i]) == 0,
			"Deserialized CD-Text titles incorrect");
	    fail_unless((deserialized_block->performers[i] == NULL &&
			 mock_block->performers[i] == NULL) ||
			strcmp(deserialized_block->performers[i],
			       mock_block->performers[i]) == 0,
			"Deserialized CD-Text performers incorrect");
	    fail_unless((deserialized_block->songwriters[i] == NULL &&
			 mock_block->songwriters[i] == NULL) ||
			strcmp(deserialized_block->songwriters[i],
			       mock_block->songwriters[i]) == 0,
			"Deserialized CD-Text songwriters incorrect");
	    fail_unless((deserialized_block->composers[i] == NULL &&
			 mock_block->composers[i] == NULL) ||
			strcmp(deserialized_block->composers[i],
			       mock_block->composers[i]) == 0,
			"Deserialized CD-Text composers incorrect");
	    fail_unless((deserialized_block->arrangers[i] == NULL &&
			 mock_block->arrangers[i] == NULL) ||
			strcmp(deserialized_block->arrangers[i],
			       mock_block->arrangers[i]) == 0,
			"Deserialized CD-Text arrangers incorrect");
	    fail_unless((deserialized_block->messages[i] == NULL &&
			 mock_block->messages[i] == NULL) ||
			strcmp(deserialized_block->messages[i],
			       mock_block->messages[i]) == 0,
			"Deserialized CD-Text messages incorrect");
	    fail_unless((deserialized_block->private[i] == NULL &&
			 mock_block->private[i] == NULL) ||
			strcmp(deserialized_block->private[i],
			       mock_block->private[i]) == 0,
			"Deserialized CD-Text private data incorrect");
	    fail_unless((deserialized_block->upc_isrcs[i] == NULL &&
			 mock_block->upc_isrcs[i] == NULL) ||
			strcmp(deserialized_block->upc_isrcs[i],
			       mock_block->upc_isrcs[i]) == 0,
			"Deserialized CD-Text UPCs/ISRCs incorrect");
	}

	fail_unless((deserialized_block->discid == NULL &&
		     mock_block->discid == NULL) ||
		    strcmp(deserialized_block->discid,
			   mock_block->discid) == 0,
		    "Deserialized CD-Text block discid incorrect");
	fail_unless(deserialized_block->genre_code == mock_block->genre_code,
		    "Deserialized CD-Text block genre code incorrect");
	fail_unless((deserialized_block->genre_name == NULL &&
		     mock_block->genre_name == NULL) ||
		    strcmp(deserialized_block->genre_name,
			   mock_block->genre_name) == 0,
		    "Deserialized CD-Text block genre name incorrect");
    }
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
							   2, 2).min == 4 &&
		cueify_cdtext_get_toc_track_interval_start(cdtext,
							   2, 2).sec == 5 &&
		cueify_cdtext_get_toc_track_interval_start(cdtext,
							   2, 2).frm == 6,
		"Interval start in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 2).min == 4 &&
		cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 2).sec == 5 &&
		cueify_cdtext_get_toc_track_interval_end(cdtext,
							 2, 2).frm == 10,
		"Interval start in CD-Text TOC did not match");
    fail_unless(cueify_cdtext_get_num_blocks(cdtext) == 3,
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
		CUEIFY_CDTEXT_CHARSET_ASCII,
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
    fail_unless(strcmp(cueify_cdtext_block_get_performer(block, 1),
		       "David Bowie") == 0,
		"Track performer in CD-Text block did not match (with tab)");
    fail_unless(strcmp(cueify_cdtext_block_get_songwriter(block,
							  CUEIFY_CDTEXT_ALBUM),
		       "ASongwriter") == 0,
		"Album songwriter in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_songwriter(block, 1),
		       "TSongwriter") == 0,
		"Track songwriter in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_songwriter(block, 2),
		       "TSongwriter") == 0,
		"Track songwriter in CD-Text block did not match (with tab)");
    fail_unless(strcmp(cueify_cdtext_block_get_composer(block,
							CUEIFY_CDTEXT_ALBUM),
		       "AlbComposer") == 0,
		"Album composer in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_composer(block, 1),
		       "TrkComposer") == 0,
		"Track composer in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_composer(block, 9),
		       "Tk2Composer") == 0,
		"Track composer in CD-Text block did not match (with tab)");
    fail_unless(strcmp(cueify_cdtext_block_get_arranger(block,
							CUEIFY_CDTEXT_ALBUM),
		       "AlbArranger") == 0,
		"Album arranger in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_arranger(block, 1),
		       "TrkArranger") == 0,
		"Track arranger in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_message(block,
						       CUEIFY_CDTEXT_ALBUM),
		       "AlbMessages") == 0,
		"Album message in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_message(block, 1),
		       "TrkMessages") == 0,
		"Track message in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_private(block,
						       CUEIFY_CDTEXT_ALBUM),
		       "OnlyPrivate") == 0,
		"Album private data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_private(block, 1),
		       "") == 0,
		"Track private data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_upc_isrc(block,
							CUEIFY_CDTEXT_ALBUM),
		       "") == 0,
		"Album UPC data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_upc_isrc(block, 1),
		       "US-RF3-02-00001") == 0,
		"Track ISRC data in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_discid(block),
		       "CK86630") == 0,
		"DiscID in CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_genre_code(block) == 32,
		"Genre code in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_genre_name(block),
		       "Genre #32") == 0,
		"Genre name in CD-Text block did not match");
}
END_TEST


START_TEST (test_french)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;
    cueify_cdtext_block *block = cueify_cdtext_get_block(cdtext, 1);

    fail_unless(block != NULL, "CD-Text block was null");
    fail_unless(cueify_cdtext_block_get_charset(block) ==
		CUEIFY_CDTEXT_CHARSET_ISO8859_1,
		"Character set of French CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_language(block) ==
		CUEIFY_CDTEXT_LANG_FRENCH,
		"Language of French CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block,
						     CUEIFY_CDTEXT_ALBUM),
		       "Pa\xC3""\xAF""en")
		== 0,
		"Album title in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block, 2),
		       "\xE2\x96\x88\xE2\x96\x80\xE2\x99\xAA\xE2\x84\x97\xE2\x80\xA0")
		== 0,
		"Track title in CD-Text block did not match");
}
END_TEST


START_TEST (test_japanese)
{
    cueify_cdtext *cdtext = (cueify_cdtext *)&mock_cdtext;
    cueify_cdtext_block *block = cueify_cdtext_get_block(cdtext, 2);

    fail_unless(block != NULL, "CD-Text block was null");
    fail_unless(cueify_cdtext_block_get_charset(block) ==
		CUEIFY_CDTEXT_CHARSET_MSJIS,
		"Character set of Japanese CD-Text block did not match");
    fail_unless(cueify_cdtext_block_get_language(block) ==
		CUEIFY_CDTEXT_LANG_JAPANESE,
		"Language of Japanese CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block,
						     CUEIFY_CDTEXT_ALBUM),
		       "\xE3\x83\x92\xE3\x83\xBC\xE3\x82\xBC\xE3\x83\xB3")
		== 0,
		"Album title in CD-Text block did not match");
    fail_unless(strcmp(cueify_cdtext_block_get_title(block, 1),
		       "\xE3\x83\x92\xE3\x83\xBC\xE3\x82\xBC\xE3\x83\xB3")
		== 0,
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
    tcase_add_test(tc_core, test_french);
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
