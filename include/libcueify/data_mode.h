/* data_mode.h - Header for CD-ROM functions which read the mode in
 * which a data track was written on a CD.
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

#ifndef _LIBCUEIFY_DATA_MODE_H
#define _LIBCUEIFY_DATA_MODE_H

#include <libcueify/device.h>
#include <libcueify/constants.h>
#include <libcueify/types.h>

#define CUEIFY_DATA_MODE_CDDA     0x00  /** Track is CD-DA (Audio) */
#define CUEIFY_DATA_MODE_MODE1    0x01  /** Track is Mode 1 */
#define CUEIFY_DATA_MODE_MODE2    0x02  /** Track is Mode 2 */
#define CUEIFY_DATA_MODE_UNKNOWN  0x0F  /** Track is in an unknown data mode */
#define CUEIFY_DATA_MODE_ERROR    0x10  /** An error occurred when reading mode */

/**
 * Read the data mode of a track on the disc in the optical disc
 * device associated with a device handle.
 *
 * @pre { d != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param track the number of the track for which the mode should be
 *              retrieved
 * @return the mode of track number track if the mode was successfully
 *         read; otherwise CUEIFY_DATA_MODE_ERROR is returned
 */
int cueify_device_read_data_mode(cueify_device *d, uint8_t track);

#endif /* _LIBCUEIFY_DATA_MODE_H */
