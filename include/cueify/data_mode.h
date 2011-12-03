/* data_mode.h - Header for CD-ROM functions which read the mode in
 * which a data track was written on a CD or the control flags of a
 * track from the CD.
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

#ifndef _CUEIFY_DATA_MODE_H
#define _CUEIFY_DATA_MODE_H

#include <cueify/device.h>
#include <cueify/constants.h>
#include <cueify/types.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** Track is CD-DA (Audio) */
#define CUEIFY_DATA_MODE_CDDA     0x00
/** Track is Mode 1 */
#define CUEIFY_DATA_MODE_MODE_1   0x01
/** Track is Mode 2 */
#define CUEIFY_DATA_MODE_MODE_2   0x02
/** Track is in an unknown data mode */
#define CUEIFY_DATA_MODE_UNKNOWN  0x0F
/** An error occurred when reading mode */
#define CUEIFY_DATA_MODE_ERROR    0x10

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


/**
 * Read the control flags of a track on a disc in an optical disc
 * (CD-ROM) device associated with a device handle.  This function
 * reads the flags directly from the track rather than relying on the
 * TOC.
 *
 * @pre { d != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param track the number of the track for which control flags should be
 *              retrieved
 * @return the control flags for track number track in d (or 0xF if
 *         they could not be read).
 */
uint8_t cueify_device_read_track_control_flags(cueify_device *d,
					       uint8_t track);

#ifdef __cplusplus
};  /* extern "C" */
#endif  /* __cplusplus */

#endif /* _CUEIFY_DATA_MODE_H */
