/* cueify.c - A utility to generate CD cuesheets from a CD.
 *
 * Copyright (c) 2010, 2011 Ian Jacobi
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
#include <time.h>
#include <libcueify/device.h>
#include <libcueify/error.h>
#include <libcueify/toc.h>
#include <libcueify/sessions.h>
#include <libcueify/full_toc.h>
#include <libcueify/cdtext.h>
#include <libcueify/mcn_isrc.h>
#include <libcueify/indices.h>
#include <libcueify/data_mode.h>
#include <libcueify/types.h>

const char * const genre_names[0x1D] = {
    "NULL",
    "Unknown",
    "Adult Contemporary",
    "Alternative Rock",
    "Childrens",
    "Classical",
    "Contemporary Christian",
    "Country",
    "Dance",
    "Easy Listening",
    "Erotic",
    "Folk",
    "Gospel",
    "Hip Hop",
    "Jazz",
    "Latin",
    "Musical",
    "New Age",
    "Opera",
    "Operetta",
    "Pop",
    "Rap",
    "Reggae",
    "Rock",
    "Rhythm and Blues",
    "Sound Effects",
    "Soundtrack",
    "Spoken Word",
    "World Music"
};

const char * const language_names[] = {
    "UNKNOWN", /* 0x00 */
    "ALBANIAN",
    "BRETON",
    "CATALAN",
    "CROATIAN",
    "WELSH",
    "CZECH",
    "DANISH",
    "GERMAN",
    "ENGLISH",
    "SPANISH",
    "ESPERANTO",
    "ESTONIAN",
    "BASQUE",
    "FAROESE",
    "FRENCH",
    "FRISIAN", /* 0x10 */
    "IRISH",
    "GAELIC",
    "GALICIAN",
    "ICELANDIC",
    "ITALIAN",
    "SAMI",
    "LATIN",
    "LATVIAN",
    "LUXEMBOURGISH",
    "LITHUANIAN",
    "HUNGARIAN",
    "MALTESE",
    "DUTCH",
    "NORWEGIAN",
    "OCCITAN",
    "POLISH", /* 0x20 */
    "PORTUGUESE",
    "ROMANIAN",
    "ROMANSH",
    "SERBIAN",
    "SLOVAK",
    "SLOVENIAN",
    "FINNISH",
    "SWEDISH",
    "TURKISH",
    "FLEMISH",
    "WALLOON",
    "",
    "",
    "",
    "",
    "", /* 0x30 */
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", /* 0x40 */
    "",
    "",
    "",
    "",
    "ZULU",
    "VIETNAMESE",
    "UZBEK",
    "URDU",
    "UKRAINIAN",
    "THAI",
    "TELUGU",
    "TATAR",
    "TAMIL",
    "TAJIK",
    "SWAHILI",
    "SRANAN_TONGO",
    "SOMALI",
    "SINHALA",
    "SHONA",
    "SERBOCROAT",
    "RUTHENIAN",
    "RUSSIAN",
    "QUECHUA",
    "PUSHTU",
    "PUNJABI",
    "PERSIAN",
    "PAPIAMENTO",
    "ORIYA",
    "NEPALI",
    "NDEBELE",
    "MARATHI",
    "MOLDAVIAN",
    "MALAYSIAN",
    "MALAGASY",
    "MACEDONIAN",
    "LAO",
    "KOREAN",
    "KHMER",
    "KAZAKH",
    "KANNADA",
    "JAPANESE",
    "INDONESIAN",
    "HINDI",
    "HEBREW",
    "HAUSA",
    "GUARANI",
    "GUJARATI",
    "GREEK",
    "GEORGIAN",
    "FULAH",
    "DARI",
    "CHUVASH",
    "CHINESE",
    "BURMESE",
    "BULGARIAN",
    "BENGALI",
    "BELARUSIAN",
    "BAMBARA",
    "AZERBAIJANI",
    "ASSAMESE",
    "ARMENIAN",
    "ARABIC",
    "AMHARIC"
};

/** Get the MSF time for an absolute LBA offset.
 *
 * @param offset an offset to get the MSF time for.
 * @return offset in MSF time, excluding the pre-gap
 */
cueify_msf_t lba_to_msf(uint32_t offset)
{
    cueify_msf_t retval;

    retval.frm = offset % 75;
    retval.sec = offset / 75 % 60;
    retval.min = offset / 75 / 60;

    return retval;
}

/** Perform a partial sum of the CDDB DiscID.
 *
 * @param n The number of seconds in a track.
 * @return The partial sum of the CDDB DiscID so far.
 */
/*
int cddb_sum(int n)
{
    int ret;

    * For backward compatibility this algorithm must not change *

    ret = 0;
    while (n > 0) {
	ret = ret + (n % 10);
	n = n / 10;
    }
    return (ret);
}
*/

/** Generate the CDDB DiscID for a given CDROM_TOC.
 *
 * @param toc A pointer to the CDROM_TOC to generate a DiscID for.
 * @return The CDDB DiscID for this disc.
 */
/*
unsigned long cddb_discid(CDROM_TOC *toc)
{
    int i,
	t = 0,
	n = 0;
    
    * For backward compatibility this algorithm must not change *

    i = 0;
    while (i < toc->LastTrack) {
	n = n + cddb_sum((toc->TrackData[i].Address[1] * 60) + toc->TrackData[i].Address[2]);
	i++;
    }
    t = ((toc->TrackData[toc->LastTrack].Address[1] * 60) + toc->TrackData[toc->LastTrack].Address[2]) - ((toc->TrackData[0].Address[1] * 60) + toc->TrackData[0].Address[2]);
    return ((n % 0xff) << 24 | t << 8 | toc->LastTrack);
}
*/

/** Write a cuesheet file to STDOUT based on the contents of an optical
 * disc (CD-ROM) device.
 *
 * @param device the device to read out a cuesheet for
 * @return 0 if succeeded.
 */
int print_cuesheet(const char *device) {
    cueify_device *dev;
    cueify_toc *toc;
    cueify_sessions *sessions;
    cueify_full_toc *fulltoc;
    cueify_cdtext *cdtext;
    cueify_indices *indices;
    char mcn_isrc[16] = "";
    size_t size;
    time_t t = time(NULL);
    char time_str[256];
    int i, block_num;
    cueify_msf_t offset, pregap;
    uint8_t cur_session = 0, has_pregap = 0;

    dev = cueify_device_new();
    if (dev == NULL) {
	return 1;
    }

    if (cueify_device_open(dev, device) == CUEIFY_OK) {
	if (device == NULL) {
	    device = cueify_device_get_default_device();
	}
	strftime(time_str, 256, "%Y-%m-%dT%H:%M:%S", gmtime(&t));
	printf("REM GENTIME \"%s\"\n"
	       "REM DRIVE \"%s\"\n",
	       time_str,
	       device);

	/* First read the last-session data. */
	sessions = cueify_sessions_new();
	if (sessions == NULL) {
	    goto error;
	}
	if (cueify_device_read_sessions(dev, sessions) == CUEIFY_OK) {
	    /* It may be the case we can't read sessions (e.g permissions)! */
	    printf("REM FIRSTSESSION %d\n"
		   "REM LASTSESSION %d\n"
		   "REM LASTSESSION TRACK %02d\n",
		   cueify_sessions_get_first_session(sessions),
		   cueify_sessions_get_last_session(sessions),
		   cueify_sessions_get_last_session_track_number(sessions));
	    if (cueify_sessions_get_last_session_control_flags(sessions) != 0){
		int control =
		    cueify_sessions_get_last_session_control_flags(sessions);

		printf("REM LASTSESSION FLAGS");

		if ((control & CUEIFY_TOC_TRACK_HAS_PREEMPHASIS) > 0) {
		    printf(" PRE");
		}
		if ((control & CUEIFY_TOC_TRACK_PERMITS_COPYING) > 0) {
		    printf(" DCP");
		}
		if ((control & CUEIFY_TOC_TRACK_IS_DATA) > 0) {
		    printf(" DATA");
		}
		if ((control & CUEIFY_TOC_TRACK_IS_QUADRAPHONIC) > 0) {
		    printf(" 4CH");
		}
		printf("\n");
	    }

	    offset =
		lba_to_msf(cueify_sessions_get_last_session_address(sessions));

	    printf("REM LASTSESSION INDEX 01 %02d:%02d:%02d\n",
		   offset.min,
		   offset.sec,
		   offset.frm);
	}

	cueify_sessions_free(sessions);

	/* Now get the full TOC data. */
	toc = cueify_toc_new();
	if (toc == NULL) {
	    goto error;
	}
	cueify_device_read_toc(dev, toc);

	fulltoc = cueify_full_toc_new();
	if (cueify_device_read_full_toc(dev, fulltoc) != CUEIFY_OK) {
	    cueify_full_toc_free(fulltoc);
	    fulltoc = NULL;
	}

	/* And the CD-Text. */
	cdtext = cueify_cdtext_new();
	if (cueify_device_read_cdtext(dev, cdtext) != CUEIFY_OK) {
	    cueify_cdtext_free(cdtext);
	    cdtext = NULL;
	}

	/* Write out the MCN. */
	size = 16;
	if (cueify_device_read_mcn(dev, mcn_isrc, &size) == CUEIFY_OK) {
	    printf("CATALOG %s\n", mcn_isrc);
	}

	/* Print any disc-level CD-Text strings. */
	if (cdtext != NULL) {
	    const char *value = NULL;
	    cueify_cdtext_block *block = NULL;
	    for (i = 0; i < cueify_cdtext_get_num_blocks(cdtext); i++) {
		block = cueify_cdtext_get_block(cdtext, i);

		value = cueify_cdtext_block_get_arranger(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM ARRANGER \"%s\"\n", value);
		    } else {
			printf("REM ARRANGER_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_composer(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM COMPOSER \"%s\"\n", value);
		    } else {
			printf("REM COMPOSER_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_discid(block);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM DISK_ID \"%s\"\n", value);
		    } else {
			printf("REM DISK_ID_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_genre_name(block);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM GENRE \"%s\"\n",
			       genre_names[
				   cueify_cdtext_block_get_genre_code(block)]);
			if (cueify_cdtext_block_get_genre_name(block)[0] !=
			    '\0') {
			    printf("REM SUPPLEMENTAL_GENRE \"%s\"\n",
				   value);
			}
		    } else {
			printf("REM GENRE_%d \"%s\"\n", i,
			       genre_names[
				   cueify_cdtext_block_get_genre_code(block)]);
			if (cueify_cdtext_block_get_genre_name(block)[0] !=
			    '\0') {
			    printf("REM SUPPLEMENTAL_GENRE_%d \"%s\"\n", i,
				   value);
			}
		    }
		}

		value = cueify_cdtext_block_get_message(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM MESSAGE \"%s\"\n", value);
		    } else {
			printf("REM MESSAGE_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_private(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM PRIVATE \"%s\"\n", value);
		    } else {
			printf("REM PRIVATE_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_performer(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("PERFORMER \"%s\"\n", value);
		    } else {
			printf("REM PERFORMER_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_songwriter(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("SONGWRITER \"%s\"\n", value);
		    } else {
			printf("REM SONGWRITER_%d \"%s\"\n", i, value);
		    }
		}

		value = cueify_cdtext_block_get_title(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("TITLE \"%s\"\n", value);
		    } else {
			printf("REM TITLE_%d \"%s\"\n", i, value);
		    }
		}

		/* Ignore TOC_INFO. */
		/* Ignore TOC_INFO2 (for now). */

		value = cueify_cdtext_block_get_upc_isrc(
		    block, CUEIFY_CDTEXT_ALBUM);
		if (value != NULL) {
		    if (i == 0) {
			printf("REM CATALOG %s\n", value);
		    } else {
			printf("REM CATALOG_%d %s\n", i, value);
		    }
		}

		/* We ARE, however, interested in the sizeInfo. */
		if (i == 0) {
		    switch (cueify_cdtext_block_get_charset(block)) {
		    case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			printf("REM CHARSET ISO-8859-1\n");
			break;
		    case CUEIFY_CDTEXT_CHARSET_ASCII:
			printf("REM CHARSET ASCII\n");
			break;
		    case CUEIFY_CDTEXT_CHARSET_MSJIS:
			printf("REM CHARSET MS-JIS\n");
			break;
		    default:
			printf("REM CHARSET 0x%02X\n",
			       cueify_cdtext_block_get_charset(block));
			break;
		    }
		    printf("REM LANGUAGE %s\n",
			   language_names[
			       cueify_cdtext_block_get_language(block)]);
		    if (cueify_cdtext_block_has_message_copyright(block) ||
			cueify_cdtext_block_has_name_copyright(block) ||
			cueify_cdtext_block_has_title_copyright(block)) {
			printf("REM COPYRIGHT");
			if (cueify_cdtext_block_has_title_copyright(block)) {
			    printf(" TITLE");
			}
			if (cueify_cdtext_block_has_name_copyright(block)) {
			    printf(" NAMES");
			}
			if (cueify_cdtext_block_has_message_copyright(block)) {
			    printf(" MESSAGE");
			}
			printf("\n");
		    }
		} else {
		    switch (cueify_cdtext_block_get_charset(block)) {
		    case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			printf("REM CHARSET_%d ISO-8859-1\n", i);
			break;
		    case CUEIFY_CDTEXT_CHARSET_ASCII:
			printf("REM CHARSET_%d ASCII\n", i);
			break;
		    case CUEIFY_CDTEXT_CHARSET_MSJIS:
			printf("REM CHARSET_%d MS-JIS\n", i);
			break;
		    default:
			printf("REM CHARSET_%d 0x%02X\n", i,
			       cueify_cdtext_block_get_charset(block));
			break;
		    }
		    printf("REM LANGUAGE_%d %s\n", i,
			   language_names[
			       cueify_cdtext_block_get_language(block)]);
		    if (cueify_cdtext_block_has_message_copyright(block) ||
			cueify_cdtext_block_has_name_copyright(block) ||
			cueify_cdtext_block_has_title_copyright(block)) {
			printf("REM COPYRIGHT_%d", i);
			if (cueify_cdtext_block_has_title_copyright(block)) {
			    printf(" TITLE");
			}
			if (cueify_cdtext_block_has_name_copyright(block)) {
			    printf(" NAMES");
			}
			if (cueify_cdtext_block_has_message_copyright(block)) {
			    printf(" MESSAGE");
			}
			printf("\n");
		    }
		}
	    }
	}

	/* And lastly the track stuff. */
	printf("FILE \"disc.bin\" BINARY\n");
	for (i = cueify_toc_get_first_track(toc);
	     i <= cueify_toc_get_last_track(toc);
	     i++) {
	    if (fulltoc != NULL &&
		cur_session != cueify_full_toc_get_track_session(fulltoc, i)) {
		if (cur_session != 0) {
		    /* Print the leadout of the last session. */
		    offset =
			cueify_full_toc_get_session_leadout_address(
			    fulltoc, cur_session);

		    /* Adjust the lead-out. */
		    if (offset.sec < 2) {
			offset.min--;
			offset.sec += 60;
		    }
		    offset.sec -= 2;

		    printf("  REM LEAD-OUT %02d:%02d:%02d\n",
			   offset.min,
			   offset.sec,
			   offset.frm);
		}

		cur_session = cueify_full_toc_get_track_session(fulltoc, i);
		printf("  REM SESSION %02d\n",
		       cur_session);

		switch (cueify_full_toc_get_session_type(fulltoc,
							 cur_session)) {
		case CUEIFY_SESSION_MODE_1:
		    printf("  REM SESSION TYPE: CD\n");
		    break;
		case CUEIFY_SESSION_CDI:
		    printf("  REM SESSION TYPE: CD-I\n");
		    break;
		case CUEIFY_SESSION_MODE_2:
		    printf("  REM SESSION TYPE: CD-XA\n");
		    break;
		default:
		    printf("  REM SESSION TYPE: UNKNOWN\n");
		    break;
		}
	    }

	    if (cueify_toc_get_track_control_flags(toc, i) &
		CUEIFY_TOC_TRACK_IS_DATA) {
		if (cueify_full_toc_get_session_type(fulltoc, cur_session) ==
		    CUEIFY_SESSION_CDI) {
		    /* CD-I...  We special case. */
		    printf("    TRACK %02d CDI/2352\n", i);
		} else {
		    switch (cueify_device_read_data_mode(dev, i)) {
		    case CUEIFY_DATA_MODE_CDDA:
			/* We shouldn't be here, but okay... */
			printf("    TRACK %02d AUDIO\n", i);
			break;
		    case CUEIFY_DATA_MODE_MODE_1:
			printf("    TRACK %02d MODE1/2352\n", i);
			break;
		    case CUEIFY_DATA_MODE_MODE_2:
			printf("    TRACK %02d MODE2/2352\n", i);
			break;
		    case CUEIFY_DATA_MODE_UNKNOWN:
		    case CUEIFY_DATA_MODE_ERROR:
		    default:
			printf("    TRACK %02d MODE1/2352\n", i);
			break;
		    }
		}
	    } else {
		printf("    TRACK %02d AUDIO\n", i);
	    }

	    if (cdtext != NULL) {
		const char *value = NULL;
		cueify_cdtext_block *block = NULL;
		for (block_num = 0;
		     block_num < cueify_cdtext_get_num_blocks(cdtext);
		     block_num++) {
		    block = cueify_cdtext_get_block(cdtext, block_num);

		    if (i < cueify_cdtext_block_get_first_track(block) ||
			cueify_cdtext_block_get_last_track(block) < i) {
			continue;
		    }

		    value = cueify_cdtext_block_get_arranger(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      REM ARRANGER \"%s\"\n", value);
			} else {
			    printf("      REM ARRANGER_%d \"%s\"\n", block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_composer(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      REM COMPOSER \"%s\"\n", value);
			} else {
			    printf("      REM COMPOSER_%d \"%s\"\n", block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_message(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      REM MESSAGE \"%s\"\n", value);
			} else {
			    printf("      REM MESSAGE_%d \"%s\"\n", block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_private(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      REM PRIVATE \"%s\"\n", value);
			} else {
			    printf("      REM PRIVATE_%d \"%s\"\n", block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_performer(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      PERFORMER \"%s\"\n", value);
			} else {
			    printf("      REM PERFORMER_%d \"%s\"\n",block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_songwriter(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      SONGWRITER \"%s\"\n", value);
			} else {
			    printf("      REM SONGWRITER_%d \"%s\"\n",
				   block_num, value);
			}
		    }

		    value = cueify_cdtext_block_get_title(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      TITLE \"%s\"\n", value);
			} else {
			    printf("      REM TITLE_%d \"%s\"\n", block_num,
				   value);
			}
		    }

		    value = cueify_cdtext_block_get_upc_isrc(block, i);
		    if (value != NULL) {
			if (block_num == 0) {
			    printf("      REM ISRC %s\n", value);
			} else {
			    printf("      REM ISRC_%d %s\n", block_num,
				   value);
			}
		    }
		}
		for (block_num = 0;
		     block_num < cueify_cdtext_get_toc_num_track_intervals(
			 cdtext, i);
		     block_num++) {
		    cueify_msf_t start, end;

		    start = cueify_cdtext_get_toc_track_interval_start(
			cdtext, i, block_num);
		    end = cueify_cdtext_get_toc_track_interval_end(
			cdtext, i, block_num);
		    printf("      REM INTERVAL %d "
			   "%02d:%02d:%02d-%02d:%02d:%02d\n",
			   block_num + 1,
			   start.min, start.sec, start.frm,
			   end.min, end.sec, end.frm);
		}
	    }

	    size = 16;
	    if (cueify_device_read_isrc(dev, i, mcn_isrc, &size) == CUEIFY_OK){
		printf("      ISRC %s\n", mcn_isrc);
	    }

	    if ((cueify_toc_get_track_control_flags(toc, i) &
		 ~CUEIFY_TOC_TRACK_IS_DATA) != 0) {
		int control = cueify_toc_get_track_control_flags(toc, i);

		printf("      FLAGS");

		if ((control & CUEIFY_TOC_TRACK_HAS_PREEMPHASIS) > 0) {
		    printf(" PRE");
		}
		if ((control & CUEIFY_TOC_TRACK_PERMITS_COPYING) > 0) {
		    printf(" DCP");
		}
		if ((control & CUEIFY_TOC_TRACK_IS_QUADRAPHONIC) > 0) {
		    printf(" 4CH");
		}
		printf("\n");
	    }

	    offset = lba_to_msf(cueify_toc_get_track_address(toc, i));

	    if (has_pregap) {
		/* Adjust the pregap */
		if (pregap.sec < 2) {
		    pregap.min--;
		    pregap.sec += 60;
		}
		pregap.sec -= 2;

		printf("      INDEX 00 %02d:%02d:%02d\n",
		       pregap.min,
		       pregap.sec,
		       pregap.frm);
		has_pregap = 0;
	    } else if (i == 1 &&
		       (offset.min != 0 ||
			offset.sec != 0 ||
			offset.frm != 0)) {
		printf("      INDEX 00 00:00:00\n");
	    }

	    printf("      INDEX 01 %02d:%02d:%02d\n",
		   offset.min,
		   offset.sec,
		   offset.frm);

	    /* Detect any other indices. */
	    indices = cueify_indices_new();
	    if (indices != NULL &&
		cueify_device_read_track_indices(dev,
						 indices, i) == CUEIFY_OK) {
		for (block_num = 0;
		     block_num < cueify_indices_get_num_indices(indices);
		     block_num++) {
		    if (cueify_indices_get_index_number(indices,
							block_num) == 0) {
			pregap = cueify_indices_get_index_offset(indices,
								 block_num);
			has_pregap = 1;
			continue;
		    } else if (cueify_indices_get_index_number(
				   indices, block_num) == 1) {
			/* Ignore index 1 (as it should be the TOC offset. */
			continue;
		    }

		    offset = cueify_indices_get_index_offset(indices,
							     block_num);
		    /* Adjust the index offset */
		    if (offset.sec < 2) {
			offset.min--;
			offset.sec += 60;
		    }
		    offset.sec -= 2;

		    printf("      INDEX %02d %02d:%02d:%02d\n",
			   block_num + 1,
			   offset.min,
			   offset.sec,
			   offset.frm);
		}
		cueify_indices_free(indices);
	    }
	}

	offset = lba_to_msf(cueify_toc_get_disc_length(toc));

	printf("  REM LEAD-OUT %02d:%02d:%02d\n",
	       offset.min,
	       offset.sec,
	       offset.frm);

	if (cdtext != NULL) {
	    cueify_cdtext_free(cdtext);
	}
	if (fulltoc != NULL) {
	    cueify_full_toc_free(fulltoc);
	}
	cueify_toc_free(toc);
    error:
	if (cueify_device_close(dev) != CUEIFY_OK) {
	    cueify_device_free(dev);
	    return 1;
	}
    }
    cueify_device_free(dev);

    return 0;
}

int main(int argc, char *argv[]) {
    char *device;

    /* Just got two command-line arguments we expect: drive letter and log. */
    if (argc != 1 && argc != 2) {
	printf("Usage: cueify [DEVICE]\n");
	return 0;
    }

    if (argc == 2) {
	device = argv[1];
    } else {
	device = NULL;
    }

    if (print_cuesheet(device)) {
	printf("There was an issue reading the cuesheet!\n");
    }

    return 0;
}
