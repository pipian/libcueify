/* device.h - Header for CD-ROM device functions.
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

#ifndef _LIBCUEIFY_DEVICE_H
#define _LIBCUEIFY_DEVICE_H

/**
 * A transparent handle for an optical disc (CD-ROM) device.
 *
 * This is returned by cueify_device_new() and is passed as the first
 * parameter to all cueify_device_*() functions.
 */
typedef void *cueify_device;


/**
 * Return a handle for a new cueify_device object.
 *
 * If no memory could be allocated, NULL is returned.
 *
 * @param device an operating system specific device identifier, or NULL
 * @return a cueify_device object, or NULL.
 */
cueify_device *cueify_device_new(const char *device);


/**
 * Release the memory allocated for the cueify_device object.
 *
 * @param d a cueify_device object created by cueify_device_new()
 */
void cueify_device_free(cueify_device *d);


/**
 * Return a human-readable error message.
 *
 * This function will return a human-readable error message describing
 * any error encountered in the most recent cueify_device_*() function
 * which used the given cueify_device object. If there were no errors
 * in the most recent cueify_device_*() call with the object, this
 * function will return the empty string.
 *
 * @param p a cueify_device object for which an error string is requested
 * @return a string containing a human-readable description of the most
 *         recent error, or the empty string if no error occurred
 */
const char *cueify_device_get_error(cueify_device *d);

#endif /* _LIBCUEIFY_DEVICE_H */
