/* sessions_private.h - Private optical disc multi-session TOC API
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

#ifndef _LIBCUEIFY_SESSIONS_PRIVATE_H
#define _LIBCUEIFY_SESSIONS_PRIVATE_H

#include <libcueify/types.h>
#include "device_private.h"

/** Internal structure to hold multi-session data. */
typedef struct {
    /** Number of the first session in the TOC. */
    uint8_t first_session_number;
    /** Number of the last session in the TOC. */
    uint8_t last_session_number;
    /** Sub-Q-channel content format of the first track in the last session. */
    uint8_t track_adr;
    /** Track control flags of the first track in the last session. */
    uint8_t track_control;
    /** Track number of the first track in the last session. */
    uint8_t track_number;
    /** Offset of the start of the first track in the last session (LBA). */
    uint32_t track_lba;
} cueify_sessions_private;


/**
 * Unportable read of the multisession TOC of the disc in the optical
 * disc device associated with a device handle.
 *
 * @param d an opened device handle
 * @param s a multisession instance to populate
 * @return CUEIFY_OK if the multisession data was successfully read;
 *         otherwise an appropriate error code is returned
 */
int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s);

#endif  /* _LIBCUEIFY_SESSIONS_PRIVATE_H */
