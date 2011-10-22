/* full_toc_private.h - Private optical disc full table of contents (TOC) API
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

#ifndef _LIBCUEIFY_FULL_TOC_PRIVATE_H
#define _LIBCUEIFY_FULL_TOC_PRIVATE_H

#include <libcueify/types.h>
#include "device_private.h"

#define MAX_TRACKS  100  /** Maximum number of tracks on a CD. */

/** Internal structure to hold track data in a full TOC. */
typedef struct {
    /** Session number. */
    uint8_t session;
    /** Sub-Q-channel content format. */
    uint8_t adr;
    /** Track control flags. */
    uint8_t control;
    /** Absolute time of the track in the TOC (MSF). */
    cueify_msf_t atime;
    /** Offset of the start of the track (MSF). */
    cueify_msf_t offset;
} cueify_full_toc_track_private;


/** Internal structure to hold full TOC data. */
typedef struct {
    /** Number of the first session in the TOC. */
    uint8_t first_session_number;
    /** Number of the last session in the TOC. */
    uint8_t last_session_number;
    /** Number of the first track in the TOC. */
    uint8_t first_track_number;
    /** Number of the last track in the TOC. */
    uint8_t last_track_number;
    /** Type of the disc of the TOC. */
    uint8_t disc_type;
    /** Tracks in the TOC. */
    cueify_full_toc_track_private tracks[MAX_TRACKS + 2];
} cueify_full_toc_private;


/**
 * Unportable read of the full TOC of the disc in the optical disc device
 * associated with a device handle.
 *
 * @param d an opened device handle
 * @param t a full TOC instance to populate
 * @return CUEIFY_OK if the full TOC was successfully read; otherwise an
 *         appropriate error code is returned
 */
int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t);

#endif  /* _LIBCUEIFY_FULL_TOC_PRIVATE_H */
