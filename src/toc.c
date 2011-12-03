/* toc.c - CD-ROM functions which read the (basic) TOC of a disc.
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
#include <cueify/toc.h>
#include <cueify/error.h>
#include "device_private.h"
#include "toc_private.h"

cueify_toc *cueify_toc_new() {
    return calloc(1, sizeof(cueify_toc_private));
}  /* cueify_toc_new */


int cueify_device_read_toc(cueify_device *d, cueify_toc *t) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (d == NULL || t == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    memset(toc, 0, sizeof(cueify_toc_private));

    return cueify_device_read_toc_unportable(dev, toc);
}  /* cueify_device_read_toc */


int cueify_toc_deserialize(cueify_toc *t, const uint8_t * const buffer,
			   size_t size) {
    cueify_toc_private *toc = (cueify_toc_private *)t;
    uint16_t toc_length;
    const uint8_t *bp;
    uint8_t offset;

    if (t == NULL || buffer == NULL) {
	return CUEIFY_ERR_BADARG;
    }
    if (size < 4) {
	return CUEIFY_ERR_TRUNCATED;
    }

    /* TOC Data Length */
    toc_length = ((buffer[0] << 8) | buffer[1]);
    if (size - 2 < toc_length) {
	return CUEIFY_ERR_TRUNCATED;
    }
    if ((toc_length - 2) % 8 != 0) {
	return CUEIFY_ERR_CORRUPTED;
    }

    memset(toc, 0, sizeof(cueify_toc_private));

    /* First Track Number */
    toc->first_track_number = buffer[2];
    /* Last Track Number */
    toc->last_track_number = buffer[3];

    /* TOC Track Descriptor(s) */
    bp = buffer + 4;
    while (bp < buffer + toc_length + 2) {
	/* Track Number */
	offset = bp[2];
	if (offset == 0xAA) {
	    offset = 0;
	}

	/* Reserved */
	bp++;
	/* ADR */
	toc->tracks[offset].adr = *bp >> 4;
	/* CONTROL */
	toc->tracks[offset].control = *bp & 0xF;
	bp++;
	/* Track Number */
	bp++;
	/* Reserved */
	bp++;
	/* Track Start Address (LBA) */
	toc->tracks[offset].lba = ((bp[0] << 24) |
				   (bp[1] << 16) |
				   (bp[2] << 8)  | 
				   (bp[3]));
	bp += 4;
    }

    return CUEIFY_OK;
}  /* cueify_toc_deserialize */


int cueify_toc_serialize(cueify_toc *t, uint8_t *buffer, size_t *size) {
    cueify_toc_private *toc = (cueify_toc_private *)t;
    uint16_t toc_length;
    uint8_t *bp;
    uint8_t track_number;
    int i;

    if (t == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    toc_length = (toc->last_track_number -
		  toc->first_track_number + 2) * 8 + 4;
    *size = toc_length;
    if (buffer == NULL) {
	return CUEIFY_OK;
    } else if (*size < toc_length) {
	return CUEIFY_ERR_TOOSMALL;
    }

    /* TOC Data Length */
    toc_length -= 2;
    buffer[0] = toc_length >> 8;
    buffer[1] = toc_length & 0xFF;

    /* First Track Number */
    buffer[2] = toc->first_track_number;
    /* Last Track Number */
    buffer[3] = toc->last_track_number;

    /* TOC Track Descriptor(s) */
    bp = buffer + 4;
    for (i = 0; i <= toc->last_track_number; i++) {
	if (i == toc->last_track_number) {
	    /* Lead-out */
	    track_number = 0;
	} else {
	    track_number = i + 1;
	}

	/* Reserved */
	*bp++ = 0;
	/* ADR */
	*bp = 0;
	*bp |= toc->tracks[track_number].adr << 4;
	/* CONTROL */
	*bp++ |= toc->tracks[track_number].control & 0x0F;
	/* Track Number (Lead-out is 0xAA) */
	*bp++ = (track_number == 0) ? 0xAA : track_number;
	/* Reserved */
	*bp++ = 0;
	/* Track Start Address (LBA) */
	*bp++ = toc->tracks[track_number].lba >> 24;
	*bp++ = (toc->tracks[track_number].lba >> 16) & 0xFF;
	*bp++ = (toc->tracks[track_number].lba >> 8) & 0xFF;
	*bp++ = toc->tracks[track_number].lba & 0xFF;
    }

    return CUEIFY_OK;
}  /* cueify_toc_serialize */


void cueify_toc_free(cueify_toc *t) {
    free(t);
}  /* cueify_toc_free */


uint8_t cueify_toc_get_first_track(cueify_toc *t) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else {
	return toc->first_track_number;
    }
}  /* cueify_toc_get_first_track */


uint8_t cueify_toc_get_last_track(cueify_toc *t) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else {
	return toc->last_track_number;
    }
}  /* cueify_toc_get_last_track */


uint8_t cueify_toc_get_track_control_flags(cueify_toc *t, uint8_t track) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].control;
    } else if (track == CUEIFY_LEAD_OUT_TRACK) {
	return toc->tracks[0].control;
    } else {
	return 0;
    }
}  /* cueify_toc_get_track_control_flags */


uint8_t cueify_toc_get_track_sub_q_channel_format(cueify_toc *t,
						  uint8_t track) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].adr;
    } else if (track == CUEIFY_LEAD_OUT_TRACK) {
	return toc->tracks[0].adr;
    } else {
	return 0;
    }
}  /* cueify_toc_get_track_sub_q_channel_format */


uint32_t cueify_toc_get_track_address(cueify_toc *t, uint8_t track) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].lba;
    } else if (track == CUEIFY_LEAD_OUT_TRACK) {
	return toc->tracks[0].lba;
    } else {
	return 0;
    }
}  /* cueify_toc_get_track_address */


uint32_t cueify_toc_get_track_length(cueify_toc *t, uint8_t track) {
    cueify_toc_private *toc = (cueify_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track < toc->last_track_number) {
	return toc->tracks[track + 1].lba - toc->tracks[track].lba;
    } else if (track == toc->last_track_number) {
	return toc->tracks[0].lba - toc->tracks[track].lba;
    } else {
	return 0;
    }
}  /* cueify_toc_get_track_length */
