/* cdtext.c - CD-ROM functions which read the CD-Text data of a disc.
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
#include <string.h>
#include <libcueify/cdtext.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "cdtext_private.h"
#include "charsets.h"

cueify_cdtext *cueify_cdtext_new() {
    return calloc(1, sizeof(cueify_cdtext_private));
}  /* cueify_cdtext_new */


int cueify_device_read_cdtext(cueify_device *d, cueify_cdtext *t) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;

    if (d == NULL || t == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    return cueify_device_read_cdtext_unportable(dev, cdtext);
}  /* cueify_device_read_cdtext */


/** Structure representing the contents of a CD-Text descriptor. */
struct cdtext_descriptor {
    uint8_t pack_type;  /** Type of the PACK */
    /** Track number or PACK element number; extension flag in the MSB. */
    uint8_t track_number;
    uint8_t sequence_number;  /** Number of this PACK in sequence. */
    /**
     * Double-byte char-code flag in the MSB;
     * BLOCK number in next 3 highest bits;
     * Character position in the 4 least significant bits.
     */
    uint8_t block_number;
    /* Data of the PACK. */
    uint8_t data[12];
    /* CRC checksum of the PACK ~(X^16 + X^12 + X^5 + 1) */
    uint8_t crc[2];
};


/** Extract the extension flag from a track_number in a cdtext_descriptor. */
#define EXTENSION_FLAG(x)  (x >> 7)
/** Extract the track number from a track_number in a cdtext_descriptor. */
#define TRACK_NUMBER(x)  (x & 0x7F)
/** Extract the double-byte flag from a block_number in a cdtext_descriptor. */
#define DOUBLE_BYTE(x)  (x >> 7)
/** Extract the block number from a block_number in a cdtext_descriptor. */
#define BLOCK_NUMBER(x)  ((x >> 4) & 0x7)
/** Extract the character offset from a block_number in a cdtext_descriptor. */
#define CHAR_POSITION(x)  (x & 0xF)


int cueify_cdtext_deserialize(cueify_cdtext *t, uint8_t *buffer,
			      size_t size) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    uint16_t toc_length;
    uint8_t *bp;
    int block, pack_type, track;
    unsigned char *pack_data[MAX_BLOCKS][16];
    unsigned char *pack;
    size_t pack_sizes[MAX_BLOCKS][16];
    size_t pack_size;
    struct cdtext_descriptor *descriptor;

    if (t == NULL || buffer == NULL) {
	return CUEIFY_ERR_BADARG;
    }
    if (size < 4) {
	return CUEIFY_ERR_TRUNCATED;
    }

    /* CD-TEXT Data Length */
    toc_length = ((buffer[0] << 8) | buffer[1]);
    if (size - 2 < toc_length) {
	return CUEIFY_ERR_TRUNCATED;
    }
    if ((toc_length - 2) % 18 != 0) {
	return CUEIFY_ERR_CORRUPTED;
    }

    /* Reserved */

    /* Zero out the pack data. */
    for (block = 0; block < MAX_BLOCKS; block++) {
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    pack_data[block][pack_type] = NULL;
	    pack_sizes[block][pack_type] = 0;
	}
    }

    /* Zero out the cdtext data (so it can be freed) */
    for (track = 0; track < MAX_TRACKS; track++) {
	for (block = 0; block < MAX_BLOCKS; block++) {
	    free(cdtext->blocks[block].titles[track]);
	    cdtext->blocks[block].titles[track] = NULL;
	    free(cdtext->blocks[block].performers[track]);
	    cdtext->blocks[block].performers[track] = NULL;
	    free(cdtext->blocks[block].songwriters[track]);
	    cdtext->blocks[block].songwriters[track] = NULL;
	    free(cdtext->blocks[block].composers[track]);
	    cdtext->blocks[block].composers[track] = NULL;
	    free(cdtext->blocks[block].arrangers[track]);
	    cdtext->blocks[block].arrangers[track] = NULL;
	    free(cdtext->blocks[block].messages[track]);
	    cdtext->blocks[block].messages[track] = NULL;
	    free(cdtext->blocks[block].private[track]);
	    cdtext->blocks[block].private[track] = NULL;
	    free(cdtext->blocks[block].upc_isrcs[track]);
	    cdtext->blocks[block].upc_isrcs[track] = NULL;
	    if (cdtext->blocks[block].discid != NULL) {
		free(cdtext->blocks[block].discid);
		cdtext->blocks[block].discid = NULL;
	    }
	    if (cdtext->blocks[block].genre_name != NULL) {
		free(cdtext->blocks[block].genre_name);
		cdtext->blocks[block].genre_name = NULL;
	    }
	}

	free(cdtext->toc.intervals[track]);
	cdtext->toc.intervals[track] = NULL;
	cdtext->toc.num_intervals[track] = 0;
    }

    /* Count all of the PACKs by BLOCK, so we know how much space to
     * allocate. */
    for (bp = buffer + 4; bp < buffer + (toc_length - 2); bp += 18) {
	descriptor = (struct cdtext_descriptor *)bp;

	/* Ignore extension blocks. */
	if (EXTENSION_FLAG(descriptor->track_number) ||
	    descriptor->pack_type < 0x80 ||
	    descriptor->pack_type > 0x8F) {
	    continue;
	}

	pack_sizes[BLOCK_NUMBER(descriptor->block_number)]
	    [descriptor->pack_type - 0x80] += 12;
    }

    /* Allocate the aggregate PACK space. */
    for (block = 0; block < MAX_BLOCKS; block++) {
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    if (pack_sizes[block][pack_type] > 0) {
		pack_data[block][pack_type] =
		    malloc(pack_sizes[block][pack_type]);
		if (pack_data[block][pack_type] == NULL) {
		    goto error;
		}
		/* Reset the size count, so that we can use it as a pointer. */
		pack_sizes[block][pack_type] = 0;
	    }
	}
    }

    /* Aggregate all PACKs. */
    /* NOTE: We assume that PACKs in a given PACK type are in order!! */
    for (bp = buffer + 4; bp < buffer + (toc_length - 2); bp += 18) {
	descriptor = (struct cdtext_descriptor *)bp;

	/* Ignore extension blocks. */
	if (EXTENSION_FLAG(descriptor->track_number) ||
	    descriptor->pack_type < 0x80 ||
	    descriptor->pack_type > 0x8F) {
	    continue;
	}

	block = BLOCK_NUMBER(descriptor->block_number);
	pack_type = descriptor->pack_type - 0x80;

	pack = pack_data[block][pack_type];
	pack_size = pack_sizes[block][pack_type];
	memcpy(pack + pack_size, descriptor->data, 12);
	pack_sizes[block][pack_type] += 12;
	/* TODO: Check CRC */
    }

    /*
     * Now that we have collected all of the PACKs by BLOCK and type,
     * parse the size information PACKs to get the character code,
     * track, and language code info.
     */
    for (block = 0; block < MAX_BLOCKS; block++) {
	/* Don't know how to handle any other size of the size
	 * information block! */
	if (pack_sizes[block][15] != 36) {
	    continue;
	}

	cdtext->blocks[block].valid = 1;
	cdtext->blocks[block].charset = pack_data[block][15][0];
	/* NOTE: This might not work if the language codes aren't in
	 * EVERY info block */
	cdtext->blocks[block].language = pack_data[block][15][block + 28];
	cdtext->blocks[block].first_track_number = pack_data[block][15][1];
	cdtext->blocks[block].last_track_number = pack_data[block][15][2];
	cdtext->blocks[block].program_cdtext =
	    (pack_data[block][15][3] >> 7) & 1;
	cdtext->blocks[block].program_copyright =
	    (pack_data[block][15][3] >> 6) & 1;
	cdtext->blocks[block].message_copyright =
	    (pack_data[block][15][3] >> 2) & 1;
	cdtext->blocks[block].name_copyright =
	    (pack_data[block][15][3] >> 1) & 1;
	cdtext->blocks[block].title_copyright =
	    (pack_data[block][15][3]) & 1;
    }

    for (block = 0; block < MAX_BLOCKS; block++) {
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    if (pack_sizes[block][pack_type] > 0) {
		char **datum;
		char *data = NULL, *data_ptr = NULL;

		switch (pack_type) {
		case 0:   /* 0x80 = TITLE */
		case 1:   /* 0x81 = PERFORMER */
		case 2:   /* 0x82 = SONGWRITER */
		case 3:   /* 0x83 = COMPOSER */
		case 4:   /* 0x84 = ARRANGER */
		case 5:   /* 0x85 = MESSAGE */
		case 13:  /* 0x8D = PRIVATE */
		case 14:  /* 0x8E = UPC/ISRC */
		    /* Textual track-wise PACKs. */

		    /* Decode the PACK contents. */
		    data = NULL;
		    switch (cdtext->blocks[block].charset) {
		    case CUEIFY_CDTEXT_CHARSET_ASCII:
		    case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			data = latin1_to_utf8(pack_data[block][pack_type],
					      pack_sizes[block][pack_type]);
			if (data == NULL) {
			    goto error;
			}
			break;
		    case CUEIFY_CDTEXT_CHARSET_MSJIS:
			/* NOTE: MS-JIS is a two-byte encoding. */
			data = msjis_to_utf8(pack_data[block][pack_type],
					     pack_sizes[block][pack_type] / 2);
			if (data == NULL) {
			    goto error;
			}
			break;
		    default:
			/* Ignore! */
			break;
		    }

		    /* Split data by track. */
		    switch (pack_type) {
		    case 0:   /* 0x80 = TITLE */
			datum = cdtext->blocks[block].titles;
			break;
		    case 1:   /* 0x81 = PERFORMER */
			datum = cdtext->blocks[block].performers;
			break;
		    case 2:   /* 0x82 = SONGWRITER */
			datum = cdtext->blocks[block].songwriters;
			break;
		    case 3:   /* 0x83 = COMPOSER */
			datum = cdtext->blocks[block].composers;
			break;
		    case 4:   /* 0x84 = ARRANGER */
			datum = cdtext->blocks[block].arrangers;
			break;
		    case 5:   /* 0x85 = MESSAGE */
			datum = cdtext->blocks[block].messages;
			break;
		    /* NOTE: 0x8D/0x8E probably break in MS-JIS!! */
		    case 13:  /* 0x8D = PRIVATE */
			datum = cdtext->blocks[block].private;
			break;
		    case 14:  /* 0x8E = UPC/ISRC */
			datum = cdtext->blocks[block].upc_isrcs;
			break;
		    default:
			break;
		    }

		    /* Copy the decoded text */
		    /* NOTE: This is probably broken for things which
		     * skip tracks. */
		    if (data != NULL) {
			data_ptr = data;
			/* First do the album-wide value. */
			datum[0] = strdup(data_ptr);
			if (datum[0] == NULL) {
			    /* Failed to strdup */
			    goto error;
			}
			/* NOTE: We assume all tracks are included! */
			for (track = cdtext->blocks[block].first_track_number;
			     track <= cdtext->blocks[block].last_track_number;
			     track++) {
			    /* Skip to next track entry. */
			    data_ptr += strlen(data_ptr) + 1;
			    /* Handle the TAB character/repeating values. */
			    if (strcmp(data_ptr, "\t") == 0) {
				if (track ==
				    cdtext->blocks[block].first_track_number) {
				    /* Dup the album-wide value. */
				    datum[track] = strdup(datum[0]);
				} else {
				    datum[track] = strdup(datum[track - 1]);
				}
			    } else {
				datum[track] = strdup(data_ptr);
			    }
			    if (datum[track] == NULL) {
				/* Failed to strdup */
				goto error;
			    }
			}

			/* All done with the decoded value. We can free it. */
			free(data);
		    }
		    break;
		case 6:   /* 0x86 = DISCID */
		    /* According to Red Book, only ISO 8859-1 may be used. */
		    cdtext->blocks[block].discid =
			latin1_to_utf8(pack_data[block][pack_type],
				       pack_sizes[block][pack_type]);
		    break;
		case 7:   /* 0x87 = GENRE */
		    /* Genre includes a genre code in addition to text. */
		    cdtext->blocks[block].genre_code =
			((uint16_t)pack_data[block][pack_type][0] << 8) |
			((uint16_t)pack_data[block][pack_type][1]);
		    /* No particular reason to believe this is Latin1 only?? */
		    cdtext->blocks[block].genre_name =
			latin1_to_utf8(pack_data[block][pack_type] + 2,
				       pack_sizes[block][pack_type] - 2);
		    break;
		case 8:   /* 0x88 = TOCINFO */
		    /* This basically encodes another (short) TOC. */
		    /* NOTE: We assume that the TOC only occurs once. */
		    cdtext->toc.first_track_number =
			pack_data[block][pack_type][0];
		    cdtext->toc.last_track_number =
			pack_data[block][pack_type][1];
		    cdtext->toc.offsets[0].min =
			pack_data[block][pack_type][3];
		    cdtext->toc.offsets[0].sec =
			pack_data[block][pack_type][4];
		    cdtext->toc.offsets[0].frm =
			pack_data[block][pack_type][5];
		    /* NOTE: We assume that the tracks are contiguous
		     * in increasing order. */
		    for (track = cdtext->toc.first_track_number;
			 track <= cdtext->toc.last_track_number;
			 track++) {
			cdtext->toc.offsets[track].min =
			    pack_data[block][pack_type]
			    [12+(track - cdtext->toc.first_track_number)*3];
			cdtext->toc.offsets[track].min =
			    pack_data[block][pack_type]
			    [12+(track - cdtext->toc.first_track_number)*3+1];
			cdtext->toc.offsets[track].min =
			    pack_data[block][pack_type]
			    [12+(track - cdtext->toc.first_track_number)*3+2];
		    }
		    break;
		case 9:   /* 0x89 = TOCINFO2 */
		    /* Need the track number from the PACK, so we
		     * ignore and handle later. */
		case 15:  /* 0x8F = SIZEINFO */
		default:
		    /* Ignore! */
		    break;
		}
	    }
	}
    }

    /* Finally, we go back and do the TOCINFO2 packets. */
    for (bp = buffer + 4; bp < buffer + (toc_length - 2); bp += 18) {
	descriptor = (struct cdtext_descriptor *)bp;

	/* Ignore extension blocks. */
	if (EXTENSION_FLAG(descriptor->track_number) ||
	    descriptor->pack_type < 0x80 ||
	    descriptor->pack_type > 0x8F) {
	    continue;
	}

	block = BLOCK_NUMBER(descriptor->block_number);
	pack_type = descriptor->pack_type;

	/* NOTE: We assume that the TOC-2 only occurs once. */
	if (pack_type == 0x89) {
	    track = TRACK_NUMBER(descriptor->track_number);

	    cdtext->toc.num_intervals[track] = descriptor->data[1];
	    if (cdtext->toc.intervals[track] == NULL) {
		cdtext->toc.intervals[track] =
		    malloc(cdtext->toc.num_intervals[track] *
			   sizeof(cueify_cdtext_toc_track_interval_private));
		if (cdtext->toc.intervals[track] == NULL) {
		    goto error;
		}
	    }

	    cdtext->toc.intervals[track][descriptor->data[0] - 1].start.min =
		descriptor->data[6];
	    cdtext->toc.intervals[track][descriptor->data[0] - 1].start.sec =
		descriptor->data[7];
	    cdtext->toc.intervals[track][descriptor->data[0] - 1].start.frm =
		descriptor->data[8];
	    cdtext->toc.intervals[track][descriptor->data[0] - 1].end.min =
		descriptor->data[9];
	    cdtext->toc.intervals[track][descriptor->data[0] - 1].end.sec =
		descriptor->data[10];
	    cdtext->toc.intervals[track][descriptor->data[0] - 1].end.frm =
		descriptor->data[11];
	}
    }

    for (block = 0; block < MAX_BLOCKS; block++) {
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    free(pack_data[block][pack_type]);
	}
    }

    return CUEIFY_OK;

error:
    for (block = 0; block < MAX_BLOCKS; block++) {
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    free(pack_data[block][pack_type]);
	}
    }

    /* Almost always a memory error. */
    return CUEIFY_ERR_NOMEM;
}  /* cueify_cdtext_deserialize */


/** A writer of CD-Text PACKs */
struct cueify_cdtext_writer {
    cueify_cdtext_private *cdtext;  /** The CD-Text being written. */
    uint8_t *bp;  /** A pointer to output buffer of the writer */
    uint8_t track;  /** The track at the start of the internal buffer. */
    uint8_t buffer[12];  /** The internal buffer. */
    size_t size;  /** The length of data in the buffer. */
    uint8_t block;  /** The number of the current block being written. */
    uint8_t seq_number;  /** The sequence number in the block being written. */
    uint8_t pack_type;  /** The type of PACK currently being written. */
    uint8_t charpos;  /** The character position/offset of the PACK. */
    uint8_t pack_count[16];  /** Counts of various PACKs written. */
    /** Expected descriptors per block. */
    uint16_t block_descriptors[MAX_BLOCKS];
};


/** Flush any unwritten contents in a CD-Text writer to its output.
 *
 * @param writer the writer to flush
 * @return CUEIFY_OK if the flush was successful
 */
static int flush_cdtext_writer(struct cueify_cdtext_writer *writer) {
    /* There's nothing to flush! */
    if (writer->size == 0) {
	return CUEIFY_OK;
    }

    /* Pad the buffer with zeroes. */
    while (writer->size < 12) {
	writer->buffer[writer->size++] = 0;
    }

    /* Do the write. */
    writer->bp[0] = writer->pack_type;  /* PACK type */
    writer->bp[1] = writer->track++;  /* Track/PACK element */
    writer->bp[2] = writer->seq_number++;  /* Sequence number */
    /* Double-byte flag, block number, and character position */
    writer->bp[3] =
	((((writer->pack_type <= 0x85 &&
	    writer->cdtext->blocks[writer->block].charset ==
	    CUEIFY_CDTEXT_CHARSET_MSJIS) ? 1 : 0) << 7) |
	 ((writer->block & 0x7) << 4) |
	 (writer->charpos));
    memcpy(writer->bp + 4, writer->buffer, 12);
    /* TODO: CRC */

    writer->bp += 18;

    /* Finish flushing state. */
    writer->size = 0;
    writer->charpos = 0;

    return CUEIFY_OK;
}  /* flush_cdtext_writer */


/** Write data as CD-Text, flushing to the writer's output as needed.
 *
 * @param writer the writer to write data to
 * @param data a pointer to the data to write
 * @param size the amount of data to write
 * @param track the track/PACK element for which data is being written
 * @return CUEIFY_OK if the write was successful
 */
static int write_cdtext_track_data(struct cueify_cdtext_writer *writer,
				   uint8_t *data, size_t size, uint8_t track) {
    uint8_t *data_ptr = data;

    if (writer->size == 0) {
	writer->charpos = 0;
	writer->track = track;
    }
    while (size >= 12 - writer->size) {
	memcpy(writer->buffer + writer->size, data_ptr, 12 - writer->size);
	data_ptr += 12 - writer->size;
	size -= 12 - writer->size;
	writer->size = 12;
	if (flush_cdtext_writer(writer) != CUEIFY_OK) {
	    return CUEIFY_ERR_INTERNAL;
	}
    }
    if (size > 0) {
	memcpy(writer->buffer + writer->size, data_ptr, size);
	writer->track = track;
	writer->charpos = data_ptr - data;
	if (writer->charpos > 15) {
	    writer->charpos = 15;
	}
	writer->size += size;
    }

    return CUEIFY_OK;
}  /* write_cdtext_track_data */


/** Write the SIZEINFO PACKs for the current CD-Text block.  Should be
 * called after all other PACKs in the block have been written.
 *
 * @param writer the writer to write SIZEINFO PACKs for
 * @return CUEIFY_OK if the write succeeded
 */
static int finish_writing_cdtext_block(struct cueify_cdtext_writer *writer) {
    uint8_t data[12];

    writer->track = 0;

    /* PACK element 1 */
    data[0] = writer->cdtext->blocks[writer->block].charset;
    data[1] = writer->cdtext->blocks[writer->block].first_track_number;
    data[2] = writer->cdtext->blocks[writer->block].last_track_number;
    data[3] = (
	((writer->cdtext->blocks[writer->block].program_cdtext & 1) << 7) |
	((writer->cdtext->blocks[writer->block].program_copyright & 1) << 6) |
	((writer->cdtext->blocks[writer->block].message_copyright & 1) << 2) |
	((writer->cdtext->blocks[writer->block].name_copyright & 1) << 1) |
	(writer->cdtext->blocks[writer->block].title_copyright & 1));
    data[4] = writer->pack_count[0];
    data[5] = writer->pack_count[1];
    data[6] = writer->pack_count[2];
    data[7] = writer->pack_count[3];
    data[8] = writer->pack_count[4];
    data[9] = writer->pack_count[5];
    data[10] = writer->pack_count[6];
    data[11] = writer->pack_count[7];
    if (write_cdtext_track_data(writer, data, 12, 0) != CUEIFY_OK) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* PACK element 2 */
    data[0] = writer->pack_count[8];
    data[1] = writer->pack_count[9];
    data[2] = writer->pack_count[10];
    data[3] = writer->pack_count[11];
    data[4] = writer->pack_count[12];
    data[5] = writer->pack_count[13];
    data[6] = writer->pack_count[14];
    data[7] = 3;  /* Always 3 PACKs of SIZEINFO */
    data[8] = writer->block_descriptors[0];
    data[9] = writer->block_descriptors[1];
    data[10] = writer->block_descriptors[2];
    data[11] = writer->block_descriptors[3];
    if (write_cdtext_track_data(writer, data, 12, 1) != CUEIFY_OK) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* PACK element 3 */
    data[0] = writer->block_descriptors[4];
    data[1] = writer->block_descriptors[5];
    data[2] = writer->block_descriptors[6];
    data[3] = writer->block_descriptors[7];
    data[4] = writer->cdtext->blocks[0].language;
    data[5] = writer->cdtext->blocks[1].language;
    data[6] = writer->cdtext->blocks[2].language;
    data[7] = writer->cdtext->blocks[3].language;
    data[8] = writer->cdtext->blocks[4].language;
    data[9] = writer->cdtext->blocks[5].language;
    data[10] = writer->cdtext->blocks[6].language;
    data[11] = writer->cdtext->blocks[7].language;
    if (write_cdtext_track_data(writer, data, 12, 2) != CUEIFY_OK) {
	return CUEIFY_ERR_INTERNAL;
    }

    return CUEIFY_OK;
}  /* finish_writing_cdtext_block */


int cueify_cdtext_serialize(cueify_cdtext *t, uint8_t *buffer,
			    size_t *size) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    uint16_t toc_length;
    uint8_t *bp, *data;
    uint16_t num_descriptors = 0, block_descriptors[MAX_BLOCKS];
    uint16_t pack_type_len;
    size_t data_size;
    int block, pack_type, track, already_wrote_track, interval;
    char **datum;
    uint8_t terminator[2] = { '\0', '\0' };
    uint8_t trk;
    struct cueify_cdtext_writer writer;

    /* NOTE: This doesn't yet enforce the rule that single-byte
     * charsets should come before double-byte charsets. */

    if (t == NULL || buffer == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    /* How many descriptors do we need? */
    for (block = 0; block < MAX_BLOCKS; block++) {
	block_descriptors[block] = 0;
	if (cdtext->blocks[block].valid) {
	    /* 3 descriptors per block (SIZEINFO) */
	    block_descriptors[block] += 3;
	}
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    pack_type_len = 0;

	    switch (pack_type) {
	    case 0:   /* 0x80 = TITLE */
	    case 1:   /* 0x81 = PERFORMER */
	    case 2:   /* 0x82 = SONGWRITER */
	    case 3:   /* 0x83 = COMPOSER */
	    case 4:   /* 0x84 = ARRANGER */
	    case 5:   /* 0x85 = MESSAGE */
	    case 13:  /* 0x8D = PRIVATE */
	    case 14:  /* 0x8E = UPC/ISRC */
		/* Textual track-wise PACKs. */

		/* Count the data length. */
		switch (pack_type) {
		case 0:   /* 0x80 = TITLE */
		    datum = cdtext->blocks[block].titles;
		    break;
		case 1:   /* 0x81 = PERFORMER */
		    datum = cdtext->blocks[block].performers;
		    break;
		case 2:   /* 0x82 = SONGWRITER */
		    datum = cdtext->blocks[block].songwriters;
		    break;
		case 3:   /* 0x83 = COMPOSER */
		    datum = cdtext->blocks[block].composers;
		    break;
		case 4:   /* 0x84 = ARRANGER */
		    datum = cdtext->blocks[block].arrangers;
		    break;
		case 5:   /* 0x85 = MESSAGE */
		    datum = cdtext->blocks[block].messages;
		    break;
		/* NOTE: 0x8D/0x8E probably break in MS-JIS!! */
		case 13:  /* 0x8D = PRIVATE */
		    datum = cdtext->blocks[block].private;
		    break;
		case 14:  /* 0x8E = UPC/ISRC */
		    datum = cdtext->blocks[block].upc_isrcs;
		    break;
		default:
		    break;
		}

		if (datum[0][0] != '\0') {
		    /* Count the number of bytes we'd need to encode
		     * the value. */
		    switch (cdtext->blocks[block].charset) {
		    case CUEIFY_CDTEXT_CHARSET_ASCII:
			pack_type_len += ascii_byte_count(datum[0]);
			break;
		    case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			pack_type_len += latin1_byte_count(datum[0]);
			break;
		    case CUEIFY_CDTEXT_CHARSET_MSJIS:
			pack_type_len += msjis_byte_count(datum[0]);
			break;
		    default:
			/* Ignore this block encoding! */
			break;
		    }
		}
		for (track = cdtext->blocks[block].first_track_number;
		     track <= cdtext->blocks[block].last_track_number;
		     track++) {
		    /* Count the number of bytes we'd need to encode
		     * the value. */
		    if (datum[track][0] != '\0' || pack_type_len > 0) {
			switch (cdtext->blocks[block].charset) {
			case CUEIFY_CDTEXT_CHARSET_ASCII:
			    if (pack_type_len == 0) {
				/* We must encode terminators for
				 * everything we don't care about. */
				pack_type_len +=
				    (track -
				     cdtext->blocks[block]
				     .first_track_number) + 1;
			    }
			    pack_type_len += ascii_byte_count(datum[track]);
			    break;
			case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			    if (pack_type_len == 0) {
				/* We must encode terminators for
				 * everything we don't care about. */
				pack_type_len +=
				    (track -
				     cdtext->blocks[block]
				     .first_track_number) + 1;
			    }
			    pack_type_len += latin1_byte_count(datum[track]);
			    break;
			case CUEIFY_CDTEXT_CHARSET_MSJIS:
			    if (pack_type_len == 0) {
				/* We must encode terminators for
				 * everything we don't care about. */
				pack_type_len +=
				    (track -
				     cdtext->blocks[block].first_track_number +
				     1) * 2;
			    }
			    pack_type_len += msjis_byte_count(datum[track]);
			    break;
			default:
			    /* Ignore this block encoding! */
			    break;
			}
		    }
		}

		/* Divide the length of all track values by length of
		 * a pack. */
		block_descriptors[block] += pack_type_len / 12 +
		    ((pack_type_len % 12 > 0) ? 1 : 0);

		break;
	    case 6:   /* 0x86 = DISCID */
		/* According to Red Book, only ISO 8859-1 may be used. */
		if (cdtext->blocks[block].discid != NULL) {
		    pack_type_len = latin1_byte_count(
			cdtext->blocks[block].discid);
		    block_descriptors[block] += pack_type_len / 12 +
			((pack_type_len % 12 > 0) ? 1 : 0);
		}
		break;
	    case 7:   /* 0x87 = GENRE */
		/* No particular reason to believe this is Latin1 only?? */
		if (cdtext->blocks[block].genre_name != NULL) {
		    pack_type_len += latin1_byte_count(
			cdtext->blocks[block].genre_name);
		}
		/* Genre includes a genre code in addition to text. */
		if (pack_type_len > 0 ||
		    cdtext->blocks[block].genre_code != 0) {
		    pack_type_len += 2;
		}
		block_descriptors[block] += pack_type_len / 12 +
		    ((pack_type_len % 12 > 0) ? 1 : 0);
		break;
	    case 8:   /* 0x88 = TOCINFO */
	    case 9:   /* 0x89 = TOCINFO2 */
	    case 15:  /* 0x8F = SIZEINFO */
	    default:
		/* Ignore! */
		break;
	    }
	}
	if (block_descriptors[block] > 256) {
	    /* Maximum of 256 descriptors per block. */
	    return CUEIFY_ERR_INVALID_CDTEXT;
	}
	num_descriptors += block_descriptors[block];
    }
    if (cdtext->toc.first_track_number != 0 &&
	cdtext->toc.last_track_number != 0) {
	if (num_descriptors == 0) {
	    /* We still need a block if there's any TOCINFO or TOCINFO2 */
	    num_descriptors += 3;
	    block_descriptors[0] += 3;
	}
	/* Encode the TOCINFO with one descriptor plus one for each track. */
	num_descriptors += (
	    cdtext->toc.last_track_number -
	    cdtext->toc.first_track_number + 2);
	block_descriptors[0] += (
	    cdtext->toc.last_track_number -
	    cdtext->toc.first_track_number + 2);
	/* Encode the TOCINFO2 with one descriptor per interval. */
	for (track = cdtext->toc.first_track_number;
	     track <= cdtext->toc.last_track_number;
	     track++) {
	    num_descriptors += cdtext->toc.num_intervals[track];
	    block_descriptors[0] += cdtext->toc.num_intervals[track];
	}
    }

    if (block_descriptors[0] > 256) {
	/* Maximum of 256 descriptors per block (including the first
	 * with its TOCINFO. */
	return CUEIFY_ERR_INVALID_CDTEXT;
    }
    if (num_descriptors > 2048) {
	/* Maximum of 2048 descriptors. */
	return CUEIFY_ERR_INVALID_CDTEXT;
    }

    toc_length = num_descriptors * 18 + 4;
    if (*size < toc_length) {
	return CUEIFY_ERR_TOOSMALL;
    }
    *size = toc_length;

    /* TOC Data Length */
    toc_length -= 2;
    buffer[0] = toc_length >> 8;
    buffer[1] = toc_length & 0xFF;

    /* Reserved */
    buffer[2] = 0;
    buffer[3] = 0;

    /* CD-TEXT Descriptor(s) */
    bp = buffer + 4;

    /* Initialize the CD-TEXT PACK writer. */
    writer.cdtext = cdtext;
    writer.bp = bp;
    writer.track = 0;
    writer.size = 0;
    for (block = 0; block < MAX_BLOCKS; block++) {
	if (block_descriptors[block] == 0) {
	    writer.block_descriptors[block] = block_descriptors[block];
	} else {
	    writer.block_descriptors[block] = block_descriptors[block] - 1;
	}
    }

    /* Iterate through each block and PACK type in turn. */
    for (block = 0; block < MAX_BLOCKS; block++) {
	writer.block = block;
	writer.seq_number = 0;
	for (pack_type = 0; pack_type < 16; pack_type++) {
	    /* Initialize the CD-Text PACK writer at the current position. */
	    writer.pack_count[pack_type] = 0;
	    writer.pack_type = pack_type + 0x80;

	    switch (pack_type) {
	    case 0:   /* 0x80 = TITLE */
	    case 1:   /* 0x81 = PERFORMER */
	    case 2:   /* 0x82 = SONGWRITER */
	    case 3:   /* 0x83 = COMPOSER */
	    case 4:   /* 0x84 = ARRANGER */
	    case 5:   /* 0x85 = MESSAGE */
	    case 13:  /* 0x8D = PRIVATE */
	    case 14:  /* 0x8E = UPC/ISRC */
		/* Textual track-wise PACKs. */
		already_wrote_track = 0;

		switch (pack_type) {
		case 0:   /* 0x80 = TITLE */
		    datum = cdtext->blocks[block].titles;
		    break;
		case 1:   /* 0x81 = PERFORMER */
		    datum = cdtext->blocks[block].performers;
		    break;
		case 2:   /* 0x82 = SONGWRITER */
		    datum = cdtext->blocks[block].songwriters;
		    break;
		case 3:   /* 0x83 = COMPOSER */
		    datum = cdtext->blocks[block].composers;
		    break;
		case 4:   /* 0x84 = ARRANGER */
		    datum = cdtext->blocks[block].arrangers;
		    break;
		case 5:   /* 0x85 = MESSAGE */
		    datum = cdtext->blocks[block].messages;
		    break;
		case 13:  /* 0x8D = PRIVATE */
		    datum = cdtext->blocks[block].private;
		    break;
		case 14:  /* 0x8E = UPC/ISRC */
		    datum = cdtext->blocks[block].upc_isrcs;
		    break;
		default:
		    break;
		}

		if (datum[0][0] != '\0') {
		    already_wrote_track = 1;
		    switch (cdtext->blocks[block].charset) {
		    case CUEIFY_CDTEXT_CHARSET_ASCII:
			data = utf8_to_ascii(datum[0], &data_size);
			if (data == NULL) {
			    goto error;
			}
			if (write_cdtext_track_data(&writer, data, data_size,
						    0) != CUEIFY_OK) {
			    free(data);
			    goto error;
			}
			free(data);
			data = NULL;
			break;
		    case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			data = utf8_to_latin1(datum[0], &data_size);
			if (data == NULL) {
			    goto error;
			}
			if (write_cdtext_track_data(&writer, data, data_size,
						    0) != CUEIFY_OK) {
			    free(data);
			    goto error;
			}
			free(data);
			data = NULL;
			break;
		    case CUEIFY_CDTEXT_CHARSET_MSJIS:
			data = utf8_to_msjis(datum[0], &data_size);
			if (data == NULL) {
			    goto error;
			}
			if (write_cdtext_track_data(&writer, data, data_size,
						    0) != CUEIFY_OK) {
			    free(data);
			    goto error;
			}
			free(data);
			data = NULL;
			break;
		    default:
			/* Ignore this block encoding! */
			break;
		    }
		}
		for (track = cdtext->blocks[block].first_track_number;
		     track <= cdtext->blocks[block].last_track_number;
		     track++) {
		    /* Count the number of bytes we'd need to encode
		     * the value. */
		    if (datum[track][0] != '\0' || already_wrote_track) {
			switch (cdtext->blocks[block].charset) {
			case CUEIFY_CDTEXT_CHARSET_ASCII:
			    if (!already_wrote_track) {
				/* We must encode terminators for
				 * everything we don't care about. */
				already_wrote_track = 1;
				data = terminator;
				for (trk = 0; trk < track; trk++) {
				    if (write_cdtext_track_data(
					    &writer, data, 1, trk) !=
					CUEIFY_OK) {
					goto error;
				    }
				    if (trk == 0) {
					trk = cdtext->blocks[block]
					    .first_track_number;
				    }
				}
				data = NULL;
			    }

			    data = utf8_to_ascii(datum[track], &data_size);
			    if (data == NULL) {
				goto error;
			    }
			    if (write_cdtext_track_data(&writer, data,
							data_size, track) !=
				CUEIFY_OK) {
				free(data);
				goto error;
			    }
			    free(data);
			    data = NULL;
			    break;
			case CUEIFY_CDTEXT_CHARSET_ISO8859_1:
			    if (!already_wrote_track) {
				/* We must encode terminators for
				 * everything we don't care about. */
				for (trk = 0; trk < track; trk++) {
				    if (write_cdtext_track_data(
					    &writer, data, 1, trk) !=
					CUEIFY_OK) {
					data = NULL;
					goto error;
				    }
				    if (trk == 0) {
					trk = cdtext->blocks[block]
					    .first_track_number;
				    }
				}
				data = NULL;
			    }

			    data = utf8_to_latin1(datum[track], &data_size);
			    if (data == NULL) {
				goto error;
			    }
			    if (write_cdtext_track_data(&writer, data,
							data_size, track) !=
				CUEIFY_OK) {
				free(data);
				goto error;
			    }
			    free(data);
			    data = NULL;
			    break;
			case CUEIFY_CDTEXT_CHARSET_MSJIS:
			    if (!already_wrote_track) {
				/* We must encode terminators for
				 * everything we don't care about. */
				already_wrote_track = 1;
				for (trk = 0; trk < track; trk++) {
				    if (write_cdtext_track_data(
					    &writer, data, 2, trk) !=
					CUEIFY_OK) {
					data = NULL;
					goto error;
				    }
				    if (trk == 0) {
					trk = cdtext->blocks[block]
					    .first_track_number;
				    }
				}
				data = NULL;
			    }

			    data = utf8_to_msjis(datum[track], &data_size);
			    if (data == NULL) {
				goto error;
			    }
			    if (write_cdtext_track_data(&writer, data,
							data_size, track) !=
				CUEIFY_OK) {
				free(data);
				goto error;
			    }
			    free(data);
			    data = NULL;
			    break;
			default:
			    /* Ignore this block encoding! */
			    break;
			}
		    }
		}

		/* Flush the remainder. */
		if (flush_cdtext_writer(&writer) != CUEIFY_OK) {
		    goto error;
		}

		break;
	    case 6:   /* 0x86 = DISCID */
		/* According to Red Book, only ISO 8859-1 may be used. */
		if (cdtext->blocks[block].discid != NULL) {
		    data = utf8_to_latin1(cdtext->blocks[block].discid,
					  &data_size);
		    if (data == NULL) {
			goto error;
		    }
		    if (write_cdtext_track_data(&writer, data, data_size, 0) !=
			CUEIFY_OK) {
			free(data);
			goto error;
		    }
		    free(data);
		    if (flush_cdtext_writer(&writer) != CUEIFY_OK) {
			goto error;
		    }
		}
		break;
	    case 7:   /* 0x87 = GENRE */
		/* No particular reason to believe this is Latin1 only?? */
		if (cdtext->blocks[block].genre_name != NULL ||
		    cdtext->blocks[block].genre_code != 0) {
		    terminator[0] =
			(cdtext->blocks[block].genre_code >> 8) & 0xFF;
		    terminator[1] =
			(cdtext->blocks[block].genre_code) & 0xFF;
		    if (write_cdtext_track_data(&writer, data, 2, 0) !=
			CUEIFY_OK) {
			goto error;
		    }
		    terminator[0] = terminator[1] = '\0';
		}
		if (cdtext->blocks[block].genre_name != NULL) {
		    data = utf8_to_latin1(cdtext->blocks[block].genre_name,
					  &data_size);
		    if (data == NULL) {
			goto error;
		    }
		    if (write_cdtext_track_data(&writer, data, data_size, 0) !=
			CUEIFY_OK) {
			free(data);
			goto error;
		    }
		    free(data);
		}
		if (cdtext->blocks[block].genre_name != NULL ||
		    cdtext->blocks[block].genre_code != 0) {
		    if (flush_cdtext_writer(&writer) != CUEIFY_OK) {
			goto error;
		    }
		}
		break;
	    case 8:   /* 0x88 = TOCINFO */
		/* One descriptor for the header, plus one for every 4
		 * tracks */
		if (block == 0 &&
		    cdtext->toc.first_track_number != 0 &&
		    cdtext->toc.last_track_number != 0) {
		    uint8_t toc_data[6];

		    toc_data[0] = cdtext->toc.first_track_number;
		    toc_data[1] = cdtext->toc.last_track_number;
		    toc_data[2] = 0;
		    toc_data[3] = cdtext->toc.offsets[0].min;
		    toc_data[4] = cdtext->toc.offsets[0].sec;
		    toc_data[5] = cdtext->toc.offsets[0].frm;

		    if (write_cdtext_track_data(&writer, toc_data, 6, 0) !=
			CUEIFY_OK) {
			goto error;
		    }
		    if (flush_cdtext_writer(&writer) != CUEIFY_OK) {
			goto error;
		    }

		    for (track = cdtext->toc.first_track_number;
			 track <= cdtext->toc.last_track_number;
			 track++) {
			uint8_t track_data[3];

			track_data[0] = cdtext->toc.offsets[track].min;
			track_data[1] = cdtext->toc.offsets[track].sec;
			track_data[2] = cdtext->toc.offsets[track].frm;

			if (write_cdtext_track_data(&writer, data, 3, track) !=
			    CUEIFY_OK) {
			    goto error;
			}
		    }
		    if (flush_cdtext_writer(&writer) != CUEIFY_OK) {
			goto error;
		    }
		}
		break;
	    case 9:   /* 0x89 = TOCINFO2 */
		/* One descriptor for each interval */
		if (block == 0 &&
		    cdtext->toc.first_track_number != 0 &&
		    cdtext->toc.last_track_number != 0) {
		    for (track = cdtext->toc.first_track_number;
			 track <= cdtext->toc.last_track_number;
			 track++) {
			if (cdtext->toc.num_intervals[track] != 0) {
			    for (interval = 1;
				 interval <= cdtext->toc.num_intervals[track];
				 interval++) {
				uint8_t interval_data[12];

				interval_data[0] = (uint8_t)interval;
				interval_data[1] =
				    cdtext->toc.num_intervals[track];
				interval_data[2] = 0;
				interval_data[3] = 0;
				interval_data[4] = 0;
				interval_data[5] = 0;
				interval_data[6] =
				    cdtext->toc.intervals[track][interval]
				    .start.min;
				interval_data[7] =
				    cdtext->toc.intervals[track][interval]
				    .start.sec;
				interval_data[8] =
				    cdtext->toc.intervals[track][interval]
				    .start.frm;
				interval_data[9] =
				    cdtext->toc.intervals[track][interval]
				    .end.min;
				interval_data[10] =
				    cdtext->toc.intervals[track][interval]
				    .end.sec;
				interval_data[11] =
				    cdtext->toc.intervals[track][interval]
				    .end.frm;

				if (write_cdtext_track_data(&writer, data,
							    12, track) !=
				    CUEIFY_OK) {
				    goto error;
				}
			    }
			}
		    }
		}
	    case 15:  /* 0x8F = SIZEINFO */
		/* Three descriptors. */
		if (finish_writing_cdtext_block(&writer) != CUEIFY_OK) {
		    goto error;
		}
		break;
	    default:
		/* Ignore! */
		break;
	    }
	}
    }

    return CUEIFY_OK;

error:
    return CUEIFY_ERR_INTERNAL;
}  /* cueify_cdtext_serialize */


void cueify_cdtext_free(cueify_cdtext *t) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    int track, block;

    for (track = 0; track < MAX_TRACKS; track++) {
	for (block = 0; block < MAX_BLOCKS; block++) {
	    free(cdtext->blocks[block].titles[track]);
	    cdtext->blocks[block].titles[track] = NULL;
	    free(cdtext->blocks[block].performers[track]);
	    cdtext->blocks[block].performers[track] = NULL;
	    free(cdtext->blocks[block].songwriters[track]);
	    cdtext->blocks[block].songwriters[track] = NULL;
	    free(cdtext->blocks[block].composers[track]);
	    cdtext->blocks[block].composers[track] = NULL;
	    free(cdtext->blocks[block].arrangers[track]);
	    cdtext->blocks[block].arrangers[track] = NULL;
	    free(cdtext->blocks[block].messages[track]);
	    cdtext->blocks[block].messages[track] = NULL;
	    free(cdtext->blocks[block].private[track]);
	    cdtext->blocks[block].private[track] = NULL;
	    free(cdtext->blocks[block].upc_isrcs[track]);
	    cdtext->blocks[block].upc_isrcs[track] = NULL;
	    if (cdtext->blocks[block].discid != NULL) {
		free(cdtext->blocks[block].discid);
		cdtext->blocks[block].discid = NULL;
	    }
	    if (cdtext->blocks[block].genre_name != NULL) {
		free(cdtext->blocks[block].genre_name);
		cdtext->blocks[block].genre_name = NULL;
	    }
	}

	free(cdtext->toc.intervals[track]);
	cdtext->toc.intervals[track] = NULL;
	cdtext->toc.num_intervals[track] = 0;
    }

    free(t);
}  /* cueify_cdtext_free */


uint8_t cueify_cdtext_get_toc_first_track(cueify_cdtext *t) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;

    if (t == NULL) {
	return 0;
    }

    return cdtext->toc.first_track_number;
}  /* cueify_cdtext_get_toc_first_track */


uint8_t cueify_cdtext_get_toc_last_track(cueify_cdtext *t) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;

    if (t == NULL) {
	return 0;
    }

    return cdtext->toc.last_track_number;
}  /* cueify_cdtext_get_toc_last_track */


cueify_msf_t cueify_cdtext_get_toc_track_offset(cueify_cdtext *t,
						uint8_t track) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    cueify_msf_t zero;

    zero.min = zero.sec = zero.frm = 0;

    if (t == NULL ||
	track < cdtext->toc.first_track_number ||
	track > cdtext->toc.last_track_number ||
	track == CUEIFY_LEAD_OUT_TRACK) {
	return zero;
    }

    if (track == CUEIFY_LEAD_OUT_TRACK) {
	return cdtext->toc.offsets[0];
    } else {
	return cdtext->toc.offsets[track];
    }
}  /* cueify_cdtext_get_toc_track_offset */


uint8_t cueify_cdtext_get_toc_num_track_intervals(cueify_cdtext *t,
						  uint8_t track) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;

    if (t == NULL ||
	track < cdtext->toc.first_track_number ||
	track > cdtext->toc.last_track_number) {
	return 0;
    }

    return cdtext->toc.num_intervals[track];
}  /* cueify_cdtext_get_toc_num_track_intervals */


cueify_msf_t cueify_cdtext_get_toc_track_interval_start(cueify_cdtext *t,
							uint8_t track,
							uint8_t interval) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    cueify_msf_t zero;

    zero.min = zero.sec = zero.frm = 0;

    if (t == NULL ||
	track < cdtext->toc.first_track_number ||
	track > cdtext->toc.last_track_number ||
	interval < cdtext->toc.num_intervals[track]) {
	return zero;
    }

    return cdtext->toc.intervals[track][interval].start;
}  /* cueify_cdtext_get_toc_track_interval_start */


cueify_msf_t cueify_cdtext_get_toc_track_interval_end(cueify_cdtext *t,
						      uint8_t track,
						      uint8_t interval) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    cueify_msf_t zero;

    zero.min = zero.sec = zero.frm = 0;

    if (t == NULL ||
	track < cdtext->toc.first_track_number ||
	track > cdtext->toc.last_track_number ||
	interval < cdtext->toc.num_intervals[track]) {
	return zero;
    }

    return cdtext->toc.intervals[track][interval].end;
}  /* cueify_cdtext_get_toc_track_interval_end */


uint8_t cueify_cdtext_get_num_blocks(cueify_cdtext *t) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    uint8_t num_blocks = 0;
    int i;

    if (t == NULL) {
	return 0;
    }

    for (i = 0; i < MAX_BLOCKS; i++) {
	if (cdtext->blocks[i].valid) {
	    num_blocks++;
	}
    }

    return num_blocks;
}  /* cueify_cdtext_get_num_blocks */


cueify_cdtext_block *cueify_cdtext_get_block(cueify_cdtext *t, uint8_t block) {
    cueify_cdtext_private *cdtext = (cueify_cdtext_private *)t;
    uint8_t num_blocks = 0;
    int i;

    if (t == NULL) {
	return NULL;
    }

    /* MIGHT not be in order, but maybe. */
    for (i = 0; i < MAX_BLOCKS; i++) {
	if (cdtext->blocks[i].valid) {
	    num_blocks++;
	    if (num_blocks > block) {
		return (cueify_cdtext_block *)&(cdtext->blocks[i]);
	    }
	}
    }

    return NULL;
}  /* cueify_cdtext_get_block */


uint8_t cueify_cdtext_block_get_first_track(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->first_track_number;
}  /* cueify_cdtext_block_get_first_track */


uint8_t cueify_cdtext_block_get_last_track(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->last_track_number;
}  /* cueify_cdtext_block_get_last_track */


uint8_t cueify_cdtext_block_get_charset(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->charset;
}  /* cueify_cdtext_block_get_charset */


uint8_t cueify_cdtext_block_get_language(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->language;
}  /* cueify_cdtext_block_get_language */


uint8_t cueify_cdtext_block_has_program_data(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->program_cdtext;
}  /* cueify_cdtext_block_has_mode2 */


uint8_t cueify_cdtext_block_has_program_copyright(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->program_copyright;
}  /* cueify_cdtext_block_has_program_copyright */


uint8_t cueify_cdtext_block_has_message_copyright(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->message_copyright;
}  /* cueify_cdtext_block_has_message_copyright */


uint8_t cueify_cdtext_block_has_name_copyright(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->name_copyright;
}  /* cueify_cdtext_block_has_name_copyright */


uint8_t cueify_cdtext_block_has_title_copyright(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->title_copyright;
}  /* cueify_cdtext_block_has_title_copyright */


const char *cueify_cdtext_block_get_title(cueify_cdtext_block *b,
					  uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->titles[track];
}  /* cueify_cdtext_block_get_title */


const char *cueify_cdtext_block_get_performer(cueify_cdtext_block *b,
					      uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->performers[track];
}  /* cueify_cdtext_block_get_performer */


const char *cueify_cdtext_block_get_songwriter(cueify_cdtext_block *b,
					       uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->songwriters[track];
}  /* cueify_cdtext_block_get_songwriter */


const char *cueify_cdtext_block_get_composer(cueify_cdtext_block *b,
					     uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->composers[track];
}  /* cueify_cdtext_block_get_composer */


const char *cueify_cdtext_block_get_arranger(cueify_cdtext_block *b,
					     uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->arrangers[track];
}  /* cueify_cdtext_block_get_arranger */


const char *cueify_cdtext_block_get_message(cueify_cdtext_block *b,
					    uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->messages[track];
}  /* cueify_cdtext_block_get_message */


const char *cueify_cdtext_block_get_private(cueify_cdtext_block *b,
					    uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->private[track];
}  /* cueify_cdtext_block_get_private */


const char *cueify_cdtext_block_get_upc_isrc(cueify_cdtext_block *b,
					     uint8_t track) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL ||
	track < block->first_track_number ||
	track > block->last_track_number ||
	track == CUEIFY_CDTEXT_ALBUM) {
	return NULL;
    }

    return block->upc_isrcs[track];
}  /* cueify_cdtext_block_get_upc_isrc */


const char *cueify_cdtext_block_get_discid(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return NULL;
    }

    return block->discid;
}  /* cueify_cdtext_block_get_discid */


uint16_t cueify_cdtext_block_get_genre_code(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->genre_code;
}  /* cueify_cdtext_block_get_genre_code */


const char *cueify_cdtext_block_get_genre_name(cueify_cdtext_block *b) {
    cueify_cdtext_block_private *block = (cueify_cdtext_block_private *)b;

    if (b == NULL) {
	return 0;
    }

    return block->genre_name;
}  /* cueify_cdtext_block_get_genre_name */
