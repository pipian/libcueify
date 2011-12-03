/* cdtext_private.h - Private CD-Text API
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

#ifndef _CUEIFY_CDTEXT_PRIVATE_H
#define _CUEIFY_CDTEXT_PRIVATE_H

#include <cueify/types.h>
#include "device_private.h"

#define MAX_TRACKS  100  /** Maximum number of tracks on a CD. */
#define MAX_BLOCKS  8  /** Maximum number of CD-Text blocks. */

/** Internal structure to hold CD-Text block data. */
typedef struct {
    int valid;  /** 1 if the data in the block is populated. */
    uint8_t charset;  /** Character set used in the CD-Text block. */
    uint8_t language;  /** Language used in the CD-Text block. */
    /** Number of the first track in the CD-Text block. */
    uint8_t first_track_number;
    /** Number of the last track in the CD-Text block. */
    uint8_t last_track_number;
    uint8_t program_cdtext;  /** 1 if there is CD-Text in the program data. */
    /** 1 if there is copyright information in the program data CD-Text. */
    uint8_t program_copyright;
    /** 1 if the messages in the CD-Text are copyrighted. */
    uint8_t message_copyright;
    /** 1 if the names in the CD-Text are copyrighted. */
    uint8_t name_copyright;
    /** 1 if the titles in the CD-Text are copyrighted. */
    uint8_t title_copyright;
    char *titles[MAX_TRACKS];  /** Titles in a CD-Text block. */
    char *performers[MAX_TRACKS];  /** Performers in a CD-Text block. */
    char *songwriters[MAX_TRACKS];  /** Songwriters in a CD-Text block. */
    char *composers[MAX_TRACKS];  /** Composers in a CD-Text block. */
    char *arrangers[MAX_TRACKS];  /** Arrangers in a CD-Text block. */
    char *messages[MAX_TRACKS];  /** Messages in a CD-Text block. */
    char *private[MAX_TRACKS];  /** Private data in a CD-Text block. */
    char *upc_isrcs[MAX_TRACKS];  /** UPC/ISRCs in a CD-Text block. */
    char *discid;  /** Disc ID in a CD-Text block. */
    uint16_t genre_code;  /** Code of the genre in a CD-Text block. */
    char *genre_name;  /** Name of the genre in a CD-Text block. */
} cueify_cdtext_block_private;


/** Internal structure to hold CD-Text TOC interval data. */
typedef struct {
    cueify_msf_t start;
    cueify_msf_t end;
} cueify_cdtext_toc_track_interval_private;


/** Internal structure to hold CD-Text data. */
typedef struct {
    /** Number of the first track in the CD-Text TOC. */
    uint8_t first_track_number;
    /** Number of the last track in the CD-Text TOC. */
    uint8_t last_track_number;
    /** Offsets of each track in the CD-Text TOC. */
    cueify_msf_t offsets[MAX_TRACKS];
    /** Number of intervals per track in the CD-Text data. */
    uint8_t num_intervals[MAX_TRACKS];
    /** Intervals stored in the CD-Text data. */
    cueify_cdtext_toc_track_interval_private *intervals[MAX_TRACKS];
} cueify_cdtext_toc_private;


/** Internal structure to hold CD-Text data. */
typedef struct {
    /** Blocks in the CD-Text data. */
    cueify_cdtext_block_private blocks[MAX_BLOCKS];
    /** TOC data in the CD-Text data. */
    cueify_cdtext_toc_private toc;
} cueify_cdtext_private;

/**
 * Unportable read of the CD-Text of the disc in the optical disc device
 * associated with a device handle.
 *
 * @param d an opened device handle
 * @param t a CD-Text instance to populate
 * @return CUEIFY_OK if the CD-Text was successfully read; otherwise an
 *         appropriate error code is returned
 */
int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t);

#endif  /* _CUEIFY_CDTEXT_PRIVATE_H */
