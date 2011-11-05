/* mcn_isrc.c - CD-ROM functions which read the Media Catalog Number
 * or ISRCs from a CD.
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
#include <libcueify/mcn_isrc.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "mcn_isrc_private.h"

int cueify_device_read_mcn(cueify_device *d, char *buffer, size_t *size) {
    cueify_device_private *dev = (cueify_device_private *)d;

    if (d == NULL || buffer == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    return cueify_device_read_mcn_unportable(dev, buffer, size);
}  /* cueify_device_read_mcn */


int cueify_device_read_isrc(cueify_device *d, uint8_t track,
			    char *buffer, size_t *size) {
    cueify_device_private *dev = (cueify_device_private *)d;

    if (d == NULL || buffer == NULL || size == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    return cueify_device_read_isrc_unportable(dev, track, buffer, size);
}  /* cueify_device_read_isrc */
