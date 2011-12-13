/* discid.c - CD-ROM discid calculation functions.
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

#include <stdio.h>
#include <stdlib.h>
#include <cueify/error.h>
#include <cueify/device.h>
#include <cueify/toc.h>
#include <cueify/sessions.h>
#include <cueify/full_toc.h>
#include "sha1.h"
#include "toc_private.h"
#include "sessions_private.h"
#include "full_toc_private.h"

/**
 * Convert an LBA absolute address to an MSF time address.
 *
 * @param lba the LBA address to convert
 * @param msf the MSF address to populate
 */
static inline void lba_to_msf(uint32_t lba, cueify_msf_t *msf) {
    msf->min = lba / 75 / 60;
    msf->sec = lba / 75 % 60;
    msf->frm = lba % 75;

    /* It really should be the MSF value. */
    msf->sec += 2;
    if (msf->sec > 59) {
	msf->min++;
	msf->sec -= 60;
    }
}  /* lba_to_msf */


/**
 * Convert an MSF time address to an LBA absolute address.
 *
 * @param msf the MSF address to convert
 * @return the LBA address
 */
static inline uint32_t msf_to_lba(cueify_msf_t msf) {
    uint32_t lba = 0;

    lba += msf.frm;
    lba += msf.sec * 75;
    lba += msf.min * 60 * 75;
    lba -= 150;  /* Lead-in */

    return lba;
}  /* msf_to_lba */


uint32_t cueify_toc_get_freedb_id(cueify_toc *t, cueify_sessions *s) {
    cueify_toc_private *toc = (cueify_toc_private *)t;
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;
    /* NOTE: May behave different in cases where the tracks start past 1 */
    uint8_t tracks = toc->last_track_number - toc->first_track_number + 1;
    uint8_t n = 0;
    uint16_t time = 0;
    int i, freedb_address;
    cueify_msf_t address;
    uint32_t leadout;

    if (sessions != NULL &&
	sessions->first_session_number != sessions->last_session_number) {
	/*
	 * Behave like MusicBrainz and adjust the lead-out for
	 * multisession discs
	 */
	tracks = sessions->track_number - toc->first_track_number;
	leadout = sessions->track_lba - 11400;
    } else {
	leadout = toc->tracks[0].lba;
    }

    for (i = 0; i < tracks; i++) {
	lba_to_msf(toc->tracks[toc->first_track_number + i].lba, &address);
	freedb_address = (address.min * 60) + address.sec;
	while (freedb_address > 0) {
	    n += freedb_address % 10;
	    freedb_address /= 10;
	}
    }

    lba_to_msf(leadout, &address);
    time = (address.min * 60) + address.sec;
    lba_to_msf(toc->tracks[toc->first_track_number].lba, &address);
    time -= (address.min * 60) + address.sec;

    return ((n % 0xff) << 24 | time << 8 | tracks);
}  /* cueify_toc_get_freedb_id */


uint32_t cueify_full_toc_get_freedb_id(cueify_full_toc *t,
				       int use_data_tracks) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    /* NOTE: May behave different in cases where the tracks start past 1 */
    uint8_t tracks = toc->last_track_number - toc->first_track_number + 1;
    uint8_t n = 0;
    uint16_t time = 0;
    int i, freedb_address;
    cueify_msf_t leadout;

    if (use_data_tracks == 0 &&
	toc->first_session_number != toc->last_session_number) {
	/* Behave like MusicBrainz and ignore the last track. */
	tracks = toc->sessions[toc->last_session_number].first_track_number -
	    toc->first_track_number;
	leadout = toc->sessions[toc->last_session_number - 1].leadout;
    } else {
	leadout = toc->sessions[toc->last_session_number].leadout;
    }

    for (i = 0; i < tracks; i++) {
	freedb_address =
	    (toc->tracks[toc->first_track_number + i].offset.min * 60) +
	    toc->tracks[toc->first_track_number + i].offset.sec;
	while (freedb_address > 0) {
	    n += freedb_address % 10;
	    freedb_address /= 10;
	}
    }

    time = (leadout.min * 60) + leadout.sec;
    time -= (toc->tracks[toc->first_track_number].offset.min * 60) +
	toc->tracks[toc->first_track_number].offset.sec;

    return ((n % 0xff) << 24 | time << 8 | tracks);
}  /* cueify_full_toc_get_freedb_id */


uint32_t cueify_device_get_freedb_id(cueify_device *d, int use_data_tracks) {
    uint32_t discid = 0;
    int supported_apis = cueify_device_get_supported_apis(d);

    if (supported_apis & CUEIFY_DEVICE_SUPPORTS_FULL_TOC) {
	cueify_full_toc *t = cueify_full_toc_new();
	if (t == NULL) {
	    return discid;
	}
	if (cueify_device_read_full_toc(d, t) == CUEIFY_OK) {
	    discid = cueify_full_toc_get_freedb_id(t, use_data_tracks);
	}
	cueify_full_toc_free(t);
	return discid;
    } else {
	cueify_toc *t = cueify_toc_new();
	cueify_sessions *s = NULL;
	if (t == NULL) {
	    return discid;
	}
	if (cueify_device_read_toc(d, t) != CUEIFY_OK) {
	    cueify_toc_free(t);
	    return discid;
	}
	if (use_data_tracks &&
	    supported_apis & CUEIFY_DEVICE_SUPPORTS_SESSIONS) {
	    s = cueify_sessions_new();
	    if (cueify_device_read_sessions(d, s) != CUEIFY_OK) {
		cueify_sessions_free(s);
		s = NULL;
	    }
	}
	discid = cueify_toc_get_freedb_id(t, s);
	if (s != NULL) {
	    cueify_sessions_free(s);
	}
	cueify_toc_free(t);
	return discid;
    }
}  /* cueify_device_get_freedb_id */

/* TODO: write tests for CD-XA, Enhanced CD, Audio CD, and also for SWIG, and a discid example */
/**
 * Encode a binary buffer using Base64 encoding.  The returned value
 * must be freed.
 *
 * @param buffer the buffer to encode
 * @param len the number of bytes to encode
 * @param extra an array of at least 3 bytes, to be used to encode
 *              values 62, 63, and for padding respectively (if NULL,
 *              the values '+', '/', and '=' from standard Base64 will
 *              be used)
 * @return the Base64 encoded representation of the first len bytes of buffer.
 */
char *base64_encode(uint8_t *buffer, size_t len, char *extra) {
    char *base64 = NULL, *bp;
    size_t i = 0;
    uint8_t residual = 0;
    char b64chars[66] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    /* Fix the final three bytes of b64chars. */
    if (extra != NULL) {
	b64chars[62] = extra[0];
	b64chars[63] = extra[1];
	b64chars[64] = extra[2];
    }

    /* 4 Base64 bytes for every group of 3 bytes */
    base64 = malloc(len / 3 * 4 + ((len % 3 > 0) ? 4 : 0) + 1);
    if (base64 == NULL) {
	return base64;
    }

    /* Iterate through buffer. */
    bp = base64;
    for (i = 0; i < len; i++) {
	switch (i % 3) {
	case 0:
	    /* No residual. */
	    residual = (buffer[i] >> 2) & 0x3F;
	    *bp++ = b64chars[residual];
	    residual = (buffer[i] & 0x03) << 4;
	    break;
	case 1:
	    /* 2 bit residual. */
	    residual |= (buffer[i] >> 4) & 0xF;
	    *bp++ = b64chars[residual];
	    residual = (buffer[i] & 0xF) << 2;
	    break;
	case 2:
	    /* 4 bit residual. */
	    residual |= (buffer[i] >> 6) & 0x3;
	    *bp++ = b64chars[residual];
	    residual = buffer[i] & 0x3F;
	    *bp++ = b64chars[residual];
	    residual = 0;
	    break;
	}
    }

    /* Fix to an integral number of bytes... */
    if (i % 3 != 0) {
	*bp++ = b64chars[residual];
    }

    /* Pad the ending to make an integral number of 4-byte chunks. */
    if ((bp - base64) % 4 != 0) {
	for (i = (bp - base64) % 4; i < 4; i++) {
	    *bp++ = b64chars[64];
	}
    }
    *bp = '\0';

    return base64;
}  /* base64_encode */


char *cueify_toc_get_musicbrainz_id(cueify_toc *t, cueify_sessions *s) {
    cueify_toc_private *toc = (cueify_toc_private *)t;
    cueify_sessions_private *sessions = (cueify_sessions_private *)s;
    char temp[9];
    SHA1_CTX sha;
    uint8_t last_track;
    uint32_t leadout;
    int i;
    uint8_t digest[SHA1_DIGEST_SIZE];

    if (sessions != NULL &&
	sessions->first_session_number != sessions->last_session_number) {
	last_track = sessions->track_number - 1;
	leadout = sessions->track_lba - 11400;
    } else if (
	sessions == NULL &&
	(toc->tracks[toc->first_track_number].control &
	 CUEIFY_TOC_TRACK_IS_DATA) == 0 &&
	(toc->tracks[toc->last_track_number].control &
	 CUEIFY_TOC_TRACK_IS_DATA) == CUEIFY_TOC_TRACK_IS_DATA) {
	/*
	 * Heuristic: If the first track is not data and the last
	 * track IS, assume this is a multi-session disc and that
	 * there is only one track in the last session.
	 */
	last_track = toc->last_track_number - 1;
	leadout = toc->tracks[toc->last_track_number].lba - 11400;
    } else {
	last_track = toc->last_track_number;
	leadout = toc->tracks[0].lba;
    }

    cueify_sha1_init(&sha);
    sprintf(temp, "%02X", toc->first_track_number);
    cueify_sha1_update(&sha, (uint8_t *)temp, 2);
    sprintf(temp, "%02X", last_track);
    cueify_sha1_update(&sha, (uint8_t *)temp, 2);
    for (i = 0; i < MAX_TRACKS; i++) {
	if (i == 0) {
	    sprintf(temp, "%08X", leadout + 150);
	} else if (i >= toc->first_track_number &&
		   i <= last_track) {
	    sprintf(temp, "%08X", toc->tracks[i].lba + 150);
	} else {
	    sprintf(temp, "%08X", 0);
	}
	cueify_sha1_update(&sha, (uint8_t *)temp, 8);
    }
    cueify_sha1_final(&sha, digest);

    return base64_encode(digest, SHA1_DIGEST_SIZE, "._-");
}  /* cueify_toc_get_musicbrainz_id */


char *cueify_full_toc_get_musicbrainz_id(cueify_full_toc *t) {
    cueify_full_toc_private *toc = (cueify_full_toc_private *)t;
    char temp[9];
    SHA1_CTX sha;
    uint8_t last_track;
    uint32_t leadout;
    int i;
    uint8_t digest[SHA1_DIGEST_SIZE];

    if (toc->first_session_number != toc->last_session_number) {
	last_track =
	    toc->sessions[toc->last_session_number - 1].last_track_number;
	leadout = msf_to_lba(
	    toc->sessions[toc->last_session_number - 1].leadout);
    } else {
	last_track = toc->last_track_number;
	leadout = msf_to_lba(toc->sessions[toc->last_session_number].leadout);
    }

    cueify_sha1_init(&sha);
    sprintf(temp, "%02X", toc->first_track_number);
    cueify_sha1_update(&sha, (uint8_t *)temp, 2);
    sprintf(temp, "%02X", last_track);
    cueify_sha1_update(&sha, (uint8_t *)temp, 2);
    for (i = 0; i < MAX_TRACKS; i++) {
	if (i == 0) {
	    sprintf(temp, "%08X", leadout + 150);
	} else if (i >= toc->first_track_number &&
		   i <= last_track) {
	    sprintf(temp, "%08X", msf_to_lba(toc->tracks[i].offset) + 150);
	} else {
	    sprintf(temp, "%08X", 0);
	}
	cueify_sha1_update(&sha, (uint8_t *)temp, 8);
    }
    cueify_sha1_final(&sha, digest);

    return base64_encode(digest, SHA1_DIGEST_SIZE, "._-");
}  /* cueify_full_toc_get_musicbrainz_id */


char *cueify_device_get_musicbrainz_id(cueify_device *d) {
    char *discid = NULL;
    int supported_apis = cueify_device_get_supported_apis(d);

    if (supported_apis & CUEIFY_DEVICE_SUPPORTS_FULL_TOC) {
	cueify_full_toc *t = cueify_full_toc_new();
	if (t == NULL) {
	    return discid;
	}
	if (cueify_device_read_full_toc(d, t) == CUEIFY_OK) {
	    discid = cueify_full_toc_get_musicbrainz_id(t);
	}
	cueify_full_toc_free(t);
	return discid;
    } else {
	cueify_toc *t = cueify_toc_new();
	cueify_sessions *s = NULL;
	if (t == NULL) {
	    return discid;
	}
	if (cueify_device_read_toc(d, t) != CUEIFY_OK) {
	    cueify_toc_free(t);
	    return discid;
	}
	if (supported_apis & CUEIFY_DEVICE_SUPPORTS_SESSIONS) {
	    s = cueify_sessions_new();
	    if (cueify_device_read_sessions(d, s) != CUEIFY_OK) {
		cueify_sessions_free(s);
		s = NULL;
	    }
	}
	discid = cueify_toc_get_musicbrainz_id(t, s);
	if (s != NULL) {
	    cueify_sessions_free(s);
	}
	cueify_toc_free(t);
	return discid;
    }
}  /* cueify_device_get_musicbrainz_id */
