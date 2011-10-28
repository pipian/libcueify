/* mcn_isrc.h - Header for CD-ROM functions which read the Media
 * Catalog Number or ISRCs from a CD.
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

#ifndef _LIBCUEIFY_MCN_ISRC_H
#define _LIBCUEIFY_MCN_ISRC_H

#include <libcueify/device.h>
#include <libcueify/constants.h>
#include <libcueify/types.h>

/**
 * Read the Media Catalog Number of the disc in the optical disc device
 * associated with a device handle.
 *
 * @pre { d != NULL }
 * @param d an opened device handle
 * @param buffer a pointer to a location to write the media catalog
 *               number to, or NULL to determine the optimal size of
 *               such a buffer
 * @param size a pointer to the size of the buffer. When called, the
 *             size must contain the maximum number of bytes that may
 *             be stored in buffer. When this function is complete,
 *             the pointer will contain the actual number of bytes
 *             serialized, or if buffer is NULL, the number of bytes
 *             needed to fully write out the media catalog number.
 * @return CUEIFY_OK if the media catalog number was successfully
 *         read; otherwise an error code is returned
 */
int cueify_device_read_mcn(cueify_device *d, char *buffer, size_t *size);


/**
 * Read the International Standard Recording Code (ISRC) of a track in
 * the disc in the optical disc device associated with a device
 * handle.
 *
 * @pre { d != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param track the number of the track for which the ISRC should be
 *              retrieved
 * @param buffer a pointer to a location to write the ISRC
 *               to, or NULL to determine the optimal size of
 *               such a buffer
 * @param size a pointer to the size of the buffer. When called, the
 *             size must contain the maximum number of bytes that may
 *             be stored in buffer. When this function is complete,
 *             the pointer will contain the actual number of bytes
 *             serialized, or if buffer is NULL, the number of bytes
 *             needed to fully write out the ISRC.
 * @return CUEIFY_OK if the ISRC was successfully
 *         read; otherwise an error code is returned
 */
int cueify_device_read_isrc(cueify_device *d, uint8_t track,
			    char *buffer, size_t *size);

#endif /* _LIBCUEIFY_MCN_ISRC_H */
