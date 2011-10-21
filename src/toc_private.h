/* toc_private.h - Private optical disc table of contents (TOC) API
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

#ifndef _LIBCUEIFY_TOC_PRIVATE_H
#define _LIBCUEIFY_TOC_PRIVATE_H

#include <libcueify/types.h>

#define MAX_TRACKS  100  /** Maximum number of tracks on a CD. */

/** Internal structure to hold track data in a TOC. */
typedef struct {
    /** Sub-Q-channel content format. */
    uint8_t adr;
    /** Track control flags. */
    uint8_t control;
    /** Offset of the start of the track (LBA). */
    uint32_t lba;
} cueify_toc_track_private;

/** Internal structure to hold TOC data. */
typedef struct {
    /** Number of the first track in the TOC. */
    uint8_t first_track_number;
    /** Number of the last track in the TOC. */
    uint8_t last_track_number;
    /** Tracks in the TOC. */
    cueify_toc_track_private tracks[MAX_TRACKS];
} cueify_toc_private;

#endif  /* _LIBCUEIFY_TOC_PRIVATE_H */
