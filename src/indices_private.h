/* indices_private.h - Private CD Track Index API
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

#ifndef _LIBCUEIFY_INDICES_PRIVATE_H
#define _LIBCUEIFY_INDICES_PRIVATE_H

#include <libcueify/types.h>
#include "device_private.h"

/** Internal structure to hold track index data. */
typedef struct {
    uint8_t num_indices;  /** Number of indices in the given track. */
    /**
     * If 1, the indices include an additional element (i.e. in
     * addition to the num_indices normal indices) representing the
     * pregap of the following track.
     */
    uint8_t has_pregap;
    cueify_msf_t *indices;  /** The indices in the track. */
} cueify_indices_private;


/** Internal structure to hold position data from an optical disc drive. */
typedef struct {
    uint8_t track;  /** The track of a position. */
    uint8_t index;  /** The index of a position. */
    cueify_msf_t abs;  /** The absolute offset of a position. */
    /** The offset of a position relative to the start of the track. */
    cueify_msf_t rel;
} cueify_position_t;


/**
 * Unportable read of position data on the disc in the optical disc
 * device associated with a device handle.
 *
 * @param d an opened device handle
 * @param track the track to read position data from
 * @param lba the absolute address (LBA) to read position data from
 * @param pos a cueify_position object to fill
 * @return CUEIFY_OK if the position was successfully read; otherwise an
 *         appropriate error code is returned
 */
int cueify_device_read_position_unportable(cueify_device_private *d,
					   uint8_t track, uint32_t lba,
					   cueify_position_t *pos);

#endif  /* _LIBCUEIFY_INDICES_PRIVATE_H */
