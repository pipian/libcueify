/* sessions.c - CD-ROM functions which read the multi-session data of
 * a disc.
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
#include <cueify/sessions.h>
#include <cueify/error.h>
#include "device_private.h"
#include "sessions_private.h"

cueify_sessions *cueify_sessions_new() {
    return calloc(1, sizeof(cueify_sessions_private));
}  /* cueify_sessions_new */


int cueify_device_read_sessions(cueify_device *d, cueify_sessions *s) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (d == NULL || s == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    memset(sessions, 0, sizeof(cueify_sessions_private));

    return cueify_device_read_sessions_unportable(dev, sessions);
}  /* cueify_device_read_sessions */


int cueify_sessions_deserialize(cueify_sessions *s,
				const uint8_t * const buffer,
				size_t size) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;
    uint16_t sessions_length;
    const uint8_t *bp;

    if (s == NULL || buffer == NULL) {
	return CUEIFY_ERR_BADARG;
    }
    if (size < 4) {
	return CUEIFY_ERR_TRUNCATED;
    }

    /* TOC Data Length */
    sessions_length = ((buffer[0] << 8) | buffer[1]);
    if (size - 2 < sessions_length) {
	return CUEIFY_ERR_TRUNCATED;
    }
    if (sessions_length != 10) {
	return CUEIFY_ERR_CORRUPTED;
    }

    memset(sessions, 0, sizeof(cueify_sessions_private));

    /* First Complete Session Number */
    sessions->first_session_number = buffer[2];
    /* Last Complete Session Number */
    sessions->last_session_number = buffer[3];

    /* TOC Track Descriptor */
    bp = buffer + 4;
    /* Reserved */
    bp++;
    /* ADR */
    sessions->track_adr = *bp >> 4;
    /* CONTROL */
    sessions->track_control = *bp & 0xF;
    bp++;
    /* First Track Number in Last Complete Session */
    sessions->track_number = *bp++;
    /* Reserved */
    bp++;
    /* Start Address of First Track in Last Session (LBA) */
    sessions->track_lba = ((bp[0] << 24) |
			   (bp[1] << 16) |
			   (bp[2] << 8)  | 
			   (bp[3]));

    return CUEIFY_OK;
}  /* cueify_sessions_deserialize */


int cueify_sessions_serialize(cueify_sessions *s, uint8_t *buffer,
			      size_t *size) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;
    uint16_t sessions_length;
    uint8_t *bp;

    if (s == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    sessions_length = 12;
    *size = sessions_length;
    if (buffer == NULL) {
	return CUEIFY_OK;
    } else if (*size < sessions_length) {
	return CUEIFY_ERR_TOOSMALL;
    }

    /* TOC Data Length */
    sessions_length -= 2;
    buffer[0] = sessions_length >> 8;
    buffer[1] = sessions_length & 0xFF;

    /* First Complete Session Number */
    buffer[2] = sessions->first_session_number;
    /* Last Complete Session Number */
    buffer[3] = sessions->last_session_number;

    /* TOC Track Descriptor */
    bp = buffer + 4;
    /* Reserved */
    *bp++ = 0;
    /* ADR */
    *bp = 0;
    *bp |= sessions->track_adr << 4;
    /* CONTROL */
    *bp++ |= sessions->track_control & 0x0F;
    /* First Track Number in Last Complete Session */
    *bp++ = sessions->track_number;
    /* Reserved */
    *bp++ = 0;
    /* Start Address of First Track in Last Session (LBA) */
    *bp++ = sessions->track_lba >> 24;
    *bp++ = (sessions->track_lba >> 16) & 0xFF;
    *bp++ = (sessions->track_lba >> 8) & 0xFF;
    *bp++ = sessions->track_lba & 0xFF;

    return CUEIFY_OK;
}  /* cueify_sessions_serialize */


void cueify_sessions_free(cueify_sessions *s) {
    free(s);
}  /* cueify_sessions_free */


uint8_t cueify_sessions_get_first_session(cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->first_session_number;
    }
}  /* cueify_sessions_get_first_session */


uint8_t cueify_sessions_get_last_session(cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->last_session_number;
    }
}  /* cueify_sessions_get_last_session */


uint8_t cueify_sessions_get_last_session_control_flags(cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->track_control;
    }
}  /* cueify_sessions_get_last_session_control_flags */


uint8_t cueify_sessions_get_last_session_sub_q_channel_format(
    cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->track_adr;
    }
}  /* cueify_sessions_get_last_session_sub_q_channel_format */


uint8_t cueify_sessions_get_last_session_track_number(cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->track_number;
    }
}  /* cueify_sessions_get_last_session_track_number */


uint32_t cueify_sessions_get_last_session_address(cueify_sessions *s) {
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;

    if (sessions == NULL) {
	return 0;
    } else {
	return sessions->track_lba;
    }
}  /* cueify_sessions_get_last_session_address */
