/* full_toc.c - CD-ROM functions which read the full TOC of a disc.
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
#include <libcueify/full_toc.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "full_toc_private.h"

cueify_full_toc *cueify_full_toc_new() {
    return calloc(1, sizeof(cueify_full_toc_private));
}  /* cueify_full_toc_new */


int cueify_device_read_full_toc(cueify_device *d, cueify_full_toc *t) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (d == NULL || t == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    return cueify_device_read_full_toc_unportable(dev, toc);
}  /* cueify_device_read_full_toc */


int cueify_full_toc_deserialize(cueify_full_toc *t, uint8_t *buffer,
				size_t size) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    uint16_t toc_length;
    uint8_t *bp;
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
    if ((toc_length - 2) % 11 != 0) {
	return CUEIFY_ERR_CORRUPTED;
    }

    /* First Complete Session Number */
    toc->first_session_number = buffer[2];
    /* Last Complete Session Number */
    toc->last_session_number = buffer[3];

    toc->first_track_number = toc->last_track_number = 0;

    /* TOC Track Descriptor(s) */
    bp = buffer + 4;
    while (bp < buffer + toc_length + 2) {
	if ((bp[1] >> 4) == 0x05) {
	    /* Ignore extra ATIP/Interval data */
	    bp += 11;
	    continue;
	}
	/* Track Number */
	offset = bp[3];
	if (offset < MAX_TRACKS) {
	    /* standard track */
	    /* Session Number */
	    toc->tracks[offset].session = *bp++;
	    /* ADR */
	    toc->tracks[offset].adr = *bp >> 4;
	    /* CONTROL */
	    toc->tracks[offset].control = *bp & 0xF;
	    bp++;
	    /* TNO */
	    bp++;
	    /* POINT */
	    bp++;
	    /* Min */
	    toc->tracks[offset].atime.min = *bp++;
	    /* Sec */
	    toc->tracks[offset].atime.sec = *bp++;
	    /* Frame */
	    toc->tracks[offset].atime.frm = *bp++;
	    /* Zero */
	    bp++;
	    /* PMIN */
	    toc->tracks[offset].offset.min = *bp++;
	    /* PSEC */
	    toc->tracks[offset].offset.sec = *bp++;
	    /* PFRAME */
	    toc->tracks[offset].offset.frm = *bp++;
	} else if (offset == 0xA0) {
	    /* POINT == 0xA0 */
	    offset = *bp++;
	    /* Session Number */
	    toc->sessions[offset].pseudotracks[1].session = offset;
	    /* ADR */
	    toc->sessions[offset].pseudotracks[1].adr = *bp >> 4;
	    /* CONTROL */
	    toc->sessions[offset].pseudotracks[1].control = *bp & 0xF;
	    bp++;
	    /* TNO */
	    bp++;
	    /* POINT */
	    bp++;
	    /* Min */
	    toc->sessions[offset].pseudotracks[1].atime.min = *bp++;
	    /* Sec */
	    toc->sessions[offset].pseudotracks[1].atime.sec = *bp++;
	    /* Frame */
	    toc->sessions[offset].pseudotracks[1].atime.frm = *bp++;
	    /* Zero */
	    bp++;
	    /* First Track Number */
	    toc->sessions[offset].first_track_number = *bp++;
	    if (toc->sessions[offset].first_track_number == 0 ||
		toc->sessions[offset].first_track_number <
		toc->first_track_number) {
		toc->first_track_number =
		    toc->sessions[offset].first_track_number;
	    }
	    /* Disc Type */
	    toc->sessions[offset].session_type = *bp++;
	    /* Reserved */
	    bp++;
	} else if (offset == 0xA1) {
	    /* POINT == 0xA1 */
	    offset = *bp++;
	    /* Session Number */
	    toc->sessions[offset].pseudotracks[2].session = offset;
	    /* ADR */
	    toc->sessions[offset].pseudotracks[2].adr = *bp >> 4;
	    /* CONTROL */
	    toc->sessions[offset].pseudotracks[2].control = *bp & 0xF;
	    bp++;
	    /* TNO */
	    bp++;
	    /* POINT */
	    bp++;
	    /* Min */
	    toc->sessions[offset].pseudotracks[2].atime.min = *bp++;
	    /* Sec */
	    toc->sessions[offset].pseudotracks[2].atime.sec = *bp++;
	    /* Frame */
	    toc->sessions[offset].pseudotracks[2].atime.frm = *bp++;
	    /* Zero */
	    bp++;
	    /* Last Track Number */
	    toc->sessions[offset].last_track_number = *bp++;
	    if (toc->sessions[offset].last_track_number == 0 ||
		toc->sessions[offset].last_track_number >
		toc->last_track_number) {
		toc->last_track_number =
		    toc->sessions[offset].last_track_number;
	    }
	    /* Reserved */
	    bp += 2;
	} else if (offset == 0xA2) {
	    /* POINT == 0xA2 */
	    /* Session Number */
	    offset = *bp++;
	    /* Session Number */
	    toc->sessions[offset].pseudotracks[0].session = offset;
	    /* ADR */
	    toc->sessions[offset].pseudotracks[0].adr = *bp >> 4;
	    /* CONTROL */
	    toc->sessions[offset].pseudotracks[0].control = *bp & 0xF;
	    bp++;
	    /* TNO */
	    bp++;
	    /* POINT */
	    bp++;
	    /* Min */
	    toc->sessions[offset].pseudotracks[0].atime.min = *bp++;
	    /* Sec */
	    toc->sessions[offset].pseudotracks[0].atime.sec = *bp++;
	    /* Frame */
	    toc->sessions[offset].pseudotracks[0].atime.frm = *bp++;
	    /* Zero */
	    bp++;
	    /* PMIN */
	    toc->sessions[offset].pseudotracks[0].offset.min = *bp++;
	    /* PSEC */
	    toc->sessions[offset].pseudotracks[0].offset.sec = *bp++;
	    /* PFRAME */
	    toc->sessions[offset].pseudotracks[0].offset.frm = *bp++;
	    toc->sessions[offset].leadout =
		toc->sessions[offset].pseudotracks[0].offset;
	}
    }

    return CUEIFY_OK;
}  /* cueify_full_toc_deserialize */


int cueify_full_toc_serialize(cueify_full_toc *t, uint8_t *buffer,
			      size_t *size) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    uint16_t toc_length;
    uint8_t *bp;
    uint8_t cur_session = 0;
    uint8_t track_number;
    int i;

    if (t == NULL || buffer == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    toc_length =
	( (toc->last_track_number   - toc->first_track_number   + 1) +
	 ((toc->last_session_number - toc->first_session_number + 1) * 3)) * 11
	+ 4;
    if (*size < toc_length) {
	return CUEIFY_ERR_TOOSMALL;
    }
    *size = toc_length;

    /* TOC Data Length */
    toc_length -= 2;
    buffer[0] = toc_length >> 8;
    buffer[1] = toc_length & 0xFF;

    /* First Complete Session Number */
    buffer[2] = toc->first_session_number;
    /* Last Complete Session Number */
    buffer[3] = toc->last_session_number;

    /* TOC Track Descriptor(s) */
    bp = buffer + 4;
    for (i = toc->first_track_number; i <= toc->last_track_number; i++) {
	if (toc->tracks[track_number].session != cur_session) {
	    /* New session.  Add session-specific lines first. */
	    cur_session = toc->tracks[track_number].session;

	    /* POINT = 0xA0 */
	    /* Session Number */
	    *bp++ = cur_session;
	    /* ADR */
	    *bp = 0;
	    *bp |= toc->sessions[cur_session].pseudotracks[1].adr << 4;
	    /* CONTROL */
	    *bp++ |= toc->sessions[cur_session].pseudotracks[1].control & 0x0F;
	    /* TNO */
	    *bp++ = 0;
	    /* POINT */
	    *bp++ = 0xA0;
	    /* Min */
	    *bp++ = toc->sessions[cur_session].pseudotracks[1].atime.min;
	    /* Sec */
	    *bp++ = toc->sessions[cur_session].pseudotracks[1].atime.sec;
	    /* Frm */
	    *bp++ = toc->sessions[cur_session].pseudotracks[1].atime.frm;
	    /* Zero */
	    *bp++ = 0;
	    /* First Track Number */
	    *bp++ = toc->sessions[cur_session].first_track_number;
	    /* Disc Type */
	    *bp++ = toc->sessions[cur_session].session_type;
	    /* Reserved */
	    *bp++ = 0;

	    /* POINT = 0xA1 */
	    /* Session Number */
	    *bp++ = cur_session;
	    /* ADR */
	    *bp = 0;
	    *bp |= toc->sessions[cur_session].pseudotracks[2].adr << 4;
	    /* CONTROL */
	    *bp++ |= toc->sessions[cur_session].pseudotracks[2].control & 0x0F;
	    /* TNO */
	    *bp++ = 0;
	    /* POINT */
	    *bp++ = 0xA1;
	    /* Min */
	    *bp++ = toc->sessions[cur_session].pseudotracks[2].atime.min;
	    /* Sec */
	    *bp++ = toc->sessions[cur_session].pseudotracks[2].atime.sec;
	    /* Frm */
	    *bp++ = toc->sessions[cur_session].pseudotracks[2].atime.frm;
	    /* Zero */
	    *bp++ = 0;
	    /* First Track Number */
	    *bp++ = toc->sessions[cur_session].last_track_number;
	    /* Reserved */
	    *bp++ = 0;
	    *bp++ = 0;

	    /* POINT = 0xA2 */
	    /* Session Number */
	    *bp++ = cur_session;
	    /* ADR */
	    *bp = 0;
	    *bp |= toc->sessions[cur_session].pseudotracks[0].adr << 4;
	    /* CONTROL */
	    *bp++ |= toc->sessions[cur_session].pseudotracks[0].control & 0x0F;
	    /* TNO */
	    *bp++ = 0;
	    /* POINT */
	    *bp++ = 0xA1;
	    /* Min */
	    *bp++ = toc->sessions[cur_session].pseudotracks[0].atime.min;
	    /* Sec */
	    *bp++ = toc->sessions[cur_session].pseudotracks[0].atime.sec;
	    /* Frm */
	    *bp++ = toc->sessions[cur_session].pseudotracks[0].atime.frm;
	    /* Zero */
	    *bp++ = 0;
	    /* PMIN */
	    *bp++ = toc->sessions[cur_session].leadout.min;
	    /* PSEC */
	    *bp++ = toc->sessions[cur_session].leadout.sec;
	    /* PFRAME */
	    *bp++ = toc->sessions[cur_session].leadout.frm;
	}

	/* Session Number */
	*bp++ = toc->tracks[track_number].session;
	/* ADR */
	*bp = 0;
	*bp |= toc->tracks[track_number].adr << 4;
	/* CONTROL */
	*bp++ |= toc->tracks[track_number].control & 0x0F;
	/* TNO */
	*bp++ = 0;
	/* POINT */
	*bp++ = track_number;
	/* Min */
	*bp++ = toc->tracks[track_number].atime.min;
	/* Sec */
	*bp++ = toc->tracks[track_number].atime.sec;
	/* Frame */
	*bp++ = toc->tracks[track_number].atime.frm;
	/* Zero */
	*bp++ = 0;
	/* PMIN */
	*bp++ = toc->tracks[track_number].offset.min;
	/* PSEC */
	*bp++ = toc->tracks[track_number].offset.sec;
	/* PFRAME */
	*bp++ = toc->tracks[track_number].offset.frm;
    }

    return CUEIFY_OK;
}  /* cueify_full_toc_serialize */


void cueify_full_toc_free(cueify_full_toc *t) {
    free(t);
}  /* cueify_full_toc_free */


uint8_t cueify_full_toc_get_first_session(cueify_full_toc *t) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else {
	return toc->first_session_number;
    }
}  /* cueify_full_toc_get_first_session */


uint8_t cueify_full_toc_get_last_session(cueify_full_toc *t) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else {
	return toc->last_session_number;
    }
}  /* cueify_full_toc_get_last_session */


uint8_t cueify_full_toc_get_track_session(cueify_full_toc *t, uint8_t track) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].session;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_track_session */


uint8_t cueify_full_toc_get_track_control_flags(cueify_full_toc *t,
						uint8_t track) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].control;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_track_control_flags */


uint8_t cueify_full_toc_get_track_sub_q_channel_format(cueify_full_toc *t,
						       uint8_t track) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].adr;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_track_sub_q_channel_format */


uint8_t cueify_full_toc_get_session_control_flags(cueify_full_toc *t,
						  uint8_t session,
						  uint8_t point) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	switch (point) {
	case CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[1].control;
	case CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[2].control;
	case CUEIFY_LEAD_OUT_TRACK:
	    return toc->sessions[session].pseudotracks[0].control;
	default:
	    return 0;
	}
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_session_control_flags */


uint8_t cueify_full_toc_get_session_sub_q_channel_format(cueify_full_toc *t,
							 uint8_t session,
							 uint8_t point) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	switch (point) {
	case CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[1].adr;
	case CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[2].adr;
	case CUEIFY_LEAD_OUT_TRACK:
	    return toc->sessions[session].pseudotracks[0].adr;
	default:
	    return 0;
	}
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_session_sub_q_channel_format */


cueify_msf_t cueify_full_toc_get_point_address(cueify_full_toc *t,
					       uint8_t session,
					       uint8_t point) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    cueify_msf_t zero;

    zero.min = 0;
    zero.sec = 0;
    zero.frm = 0;

    if (toc == NULL) {
	return zero;
    } else if (point >= toc->first_track_number &&
	       point <= toc->last_track_number) {
	return toc->tracks[point].atime;  /* session doesn't actually matter */
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	switch (point) {
	case CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[1].atime;
	case CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK:
	    return toc->sessions[session].pseudotracks[2].atime;
	case CUEIFY_LEAD_OUT_TRACK:
	    return toc->sessions[session].pseudotracks[0].atime;
	default:
	    return zero;
	}
    } else {
	return zero;
    }
}  /* cueify_full_toc_get_point_address */


cueify_msf_t cueify_full_toc_get_track_address(cueify_full_toc *t,
					       uint8_t track) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    cueify_msf_t zero;

    zero.min = 0;
    zero.sec = 0;
    zero.frm = 0;

    if (toc == NULL) {
	return zero;
    } else if (track >= toc->first_track_number &&
	       track <= toc->last_track_number) {
	return toc->tracks[track].offset;
    } else {
	return zero;
    }
}  /* cueify_full_toc_get_track_address */


uint8_t cueify_full_toc_get_session_first_track(cueify_full_toc *t,
						uint8_t session) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	return toc->sessions[session].first_track_number;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_session_first_track */


uint8_t cueify_full_toc_get_session_last_track(cueify_full_toc *t,
					       uint8_t session) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	return toc->sessions[session].last_track_number;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_session_last_track */


uint8_t cueify_full_toc_get_session_type(cueify_full_toc *t,
					 uint8_t session) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;

    if (toc == NULL) {
	return 0;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	return toc->sessions[session].session_type;
    } else {
	return 0;
    }
}  /* cueify_full_toc_get_session_type */


cueify_msf_t cueify_full_toc_get_session_leadout_address(cueify_full_toc *t,
							 uint8_t session) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    cueify_msf_t zero;

    zero.min = 0;
    zero.sec = 0;
    zero.frm = 0;

    if (toc == NULL) {
	return zero;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	return toc->sessions[session].leadout;
    } else {
	return zero;
    }
}  /* cueify_full_toc_get_session_leadout_address */


cueify_msf_t cueify_full_toc_get_track_length(cueify_full_toc *t,
					      uint8_t track) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    cueify_msf_t diff;

    diff.min = 0;
    diff.sec = 0;
    diff.frm = 0;

    if (toc == NULL) {
	return diff;
    } else if (track >= toc->first_track_number &&
	       track < toc->last_track_number) {
	diff = toc->tracks[track + 1].offset;
	diff.min -= toc->tracks[track].offset.min;
	if (diff.sec < toc->tracks[track].offset.sec) {
	    diff.sec += 60;
	    diff.min--;
	}
	diff.sec -= toc->tracks[track].offset.sec;
	if (diff.frm < toc->tracks[track].offset.frm) {
	    diff.frm += 75;
	    if (diff.sec == 0) {
		diff.sec += 60;
		diff.min--;
	    }
	    diff.sec--;
	}
	diff.frm -= toc->tracks[track].offset.frm;
	return diff;
    } else if (track == toc->last_track_number) {
	diff = toc->tracks[0].offset;
	diff.min -= toc->tracks[track].offset.min;
	if (diff.sec < toc->tracks[track].offset.sec) {
	    diff.sec += 60;
	    diff.min--;
	}
	diff.sec -= toc->tracks[track].offset.sec;
	if (diff.frm < toc->tracks[track].offset.frm) {
	    diff.frm += 75;
	    if (diff.sec == 0) {
		diff.sec += 60;
		diff.min--;
	    }
	    diff.sec--;
	}
	diff.frm -= toc->tracks[track].offset.frm;
	return diff;
    } else {
	return diff;
    }
}  /* cueify_full_toc_get_track_length */


cueify_msf_t cueify_full_toc_get_session_length(cueify_full_toc *t,
						uint8_t session) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    cueify_msf_t start, diff;

    diff.min = 0;
    diff.sec = 0;
    diff.frm = 0;

    if (toc == NULL) {
	return diff;
    } else if (session >= toc->first_session_number &&
	       session <= toc->last_session_number) {
	diff = toc->sessions[session].leadout;
	start = toc->tracks[toc->sessions[session].first_track_number].offset;
	diff.min -= start.min;
	if (diff.sec < start.sec) {
	    diff.sec += 60;
	    diff.min--;
	}
	diff.sec -= start.sec;
	if (diff.frm < start.frm) {
	    diff.frm += 75;
	    if (diff.sec == 0) {
		diff.sec += 60;
		diff.min--;
	    }
	    diff.sec--;
	}
	diff.frm -= start.frm;
	return diff;
    } else {
	return diff;
    }
}  /* cueify_full_toc_get_session_length */
