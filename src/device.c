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
#include <string.h>
#include <libcueify/device.h>
#include <libcueify/error.h>
#include "device_private.h"

cueify_device *cueify_device_new() {
    return calloc(1, sizeof(cueify_device_private));
}  /* cueify_device_new */


int cueify_device_open(cueify_device *d, const char *device) {
    cueify_device_private *dev = (cueify_device_private *)d;

    /* Must have a defined device instance */
    if (dev == NULL) {
	return CUEIFY_ERR_BADARG;
    }
    /* Get the default device if needed */
    if (device == NULL) {
	device = cueify_device_get_default_device();
	if (device == NULL) {
	    return CUEIFY_ERR_NO_DEVICE;
	}
    }
    /* The device should actually be something */
    if (device[0] == '\0') {
	return CUEIFY_ERR_BADARG;
    }

    memset(dev, 0, sizeof(cueify_device_private));
    dev->path = malloc(strlen(device) + 1);
    if (dev->path == NULL) {
	return CUEIFY_ERR_NOMEM;
    }

    return cueify_device_open_unportable(dev, device);
}  /* cueify_device_open */


int cueify_device_close(cueify_device *d) {
    cueify_device_private *dev = (cueify_device_private *)d;

    if (dev == NULL) {
	return CUEIFY_ERR_BADARG;
    }

    return cueify_device_close_unportable(dev);
}  /* cueify_device_close */


void cueify_device_free(cueify_device *d) {
    free(d);
}  /* cueify_device_free */


const char *cueify_device_get_default_device() {
    return cueify_device_get_default_device_unportable();
}  /* cueify_device_get_default_device */
