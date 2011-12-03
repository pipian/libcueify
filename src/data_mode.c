/* data_mode.c - CD-ROM functions which read the mode in which a data
 * track was written on a CD or the control flags of a track from the
 * CD.
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
#include <cueify/data_mode.h>
#include <cueify/error.h>
#include "device_private.h"
#include "toc_private.h"

int cueify_device_read_data_mode(cueify_device *d, uint8_t track) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_toc_private toc;
    cueify_raw_read_private buffer;

    if (cueify_device_read_toc_unportable(dev, &toc) != CUEIFY_OK) {
	return CUEIFY_DATA_MODE_ERROR;
    }

    if (track >= toc.first_track_number &&
	track <= toc.last_track_number) {
	if (toc.tracks[track].control & CUEIFY_TOC_TRACK_IS_DATA) {
	    if (cueify_device_read_raw_unportable(dev, toc.tracks[track].lba,
						  &buffer) != CUEIFY_OK) {
		return CUEIFY_DATA_MODE_ERROR;
	    }
	    return buffer.data_mode;
	} else {
	    return CUEIFY_DATA_MODE_CDDA;
	}
    } else {
	return CUEIFY_DATA_MODE_ERROR;
    }
}  /* cueify_device_read_data_mode */


uint8_t cueify_device_read_track_control_flags(cueify_device *d,
					       uint8_t track) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_toc_private toc;
    cueify_raw_read_private buffer;

    if (cueify_device_read_toc_unportable(dev, &toc) != CUEIFY_OK) {
	return 0xF;
    }

    if (cueify_device_read_raw_unportable(dev, toc.tracks[track].lba,
					  &buffer) != CUEIFY_OK) {
	return 0xF;
    }

    if (sizeof(buffer) == 2352 + 16) {
	/* Must support sub-Q-channel. */
	return buffer.control_adr >> 4;
    } else {
	return 0xF;
    }
}  /* cueify_device_read_track_control_flags */
