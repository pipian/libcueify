/* device.c - CD-ROM device functions.
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
#include "libcueify/device.h"
#include "libcueify/device_private.h"

cueify_device *cueify_device_new() {
    return calloc(1, sizeof(cueify_device_private));
}  /* cueify_device_new */


int cueify_device_open(cueify_device *d, const char *device) {
    cueify_device_private *dev = (cueify_device_private *)d;

    if (dev == NULL) {
	return CUEIFY_BADARG;
    }
    if (device == NULL) {
	device = cueify_device_get_default_device();
    }

    memset(dev, 0, sizeof(cueify_device_private));

    return cueify_device_open_unportable(d, device);
}  /* cueify_device_open */


int cueify_device_close(cueify_device *d) {
    cueify_device_private *dev = (cueify_device_private *)d;

    if (dev == NULL) {
	return CUEIFY_BADARG;
    }

    return cueify_device_close_unportable(d);
}  /* cueify_device_close */


void cueify_device_free(cueify_device *d) {
    free(d);
}  /* cueify_device_free */


const char *cueify_device_get_default_device() {
    return cueify_device_get_default_device_unportable();
}  /* cueify_device_get_default_device */