/* mcn_isrc_private.h - Private Media Catalog Number/ISRC API
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

#ifndef _LIBCUEIFY_MCN_ISRC_PRIVATE_H
#define _LIBCUEIFY_MCN_ISRC_PRIVATE_H

#include <libcueify/types.h>
#include "mcn_isrc_private.h"

/**
 * Unportable read of the Media Catalog Number of the disc in the
 * optical disc device associated with a device handle.
 *
 * @param d an opened device handle
 * @param buffer a pointer to a buffer in which to write the Media
 *               Catalog Number of the CD
 * @param size a pointer to the size of the buffer to populate, will
 *             be set to the number of bytes set in the buffer on output
 * @return CUEIFY_OK if the Media Catalog Number was successfully
 *         read; CUEIFY_NO_DATA if no Media Catalog Number is present,
 *         otherwise an appropriate error code is returned
 */
int cueify_device_read_mcn_unportable(cueify_device_private *d,
				      char *buffer, size_t *size);


/**
 * Unportable read of the ISRC of a track on the disc in the
 * optical disc device associated with a device handle.
 *
 * @param d an opened device handle
 * @param track the number of the track to read the ISRC for
 * @param buffer a pointer to a buffer in which to write the ISRC
 *               of the track
 * @param size a pointer to the size of the buffer to populate, will
 *             be set to the number of bytes set in the buffer on output
 * @return CUEIFY_OK if the ISRC was successfully read; CUEIFY_NO_DATA
 *         if no ISRC is present, otherwise an appropriate error code
 *         is returned
 */
int cueify_device_read_isrc_unportable(cueify_device_private *d, uint8_t track,
				       char *buffer, size_t *size);

#endif  /* _LIBCUEIFY_CDTEXT_PRIVATE_H */
