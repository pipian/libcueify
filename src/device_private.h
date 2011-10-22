/* device_private.h - Private optical disc device API
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

#ifndef _LIBCUEIFY_DEVICE_PRIVATE_H
#define _LIBCUEIFY_DEVICE_PRIVATE_H

/** OS-specific device handle types. */
#ifdef _WIN32
#define device_handle HANDLE  /* HANDLE to an OpenVolume */
#elif defined __unix__
#define device_handle int     /* file descriptor */
#endif

/** Internal version of the cueify_device structure. */
typedef struct {
    device_handle handle;  /** OS-specific device handle */
    char *path;  /** OS-specific identifier used to open the handle. */
} cueify_device_private;

/** Unportable version of cueify_device_open().
 *
 * @param d the cueify device handle to open
 * @param device the OS-specific device identifier of the device to open
 * @return CUEIFY_OK if the device opened successfully;
 *         CUEIFY_ERR_NO_DEVICE if no such device exists or the device
 *         could not be opened; otherwise, another appropriate error
 *         code.
 */
int cueify_device_open_unportable(cueify_device_private *d,
				  const char *device);


/** Unportable version of cueify_device_close().
 *
 * @param d the cueify device handle to close
 * @return CUEIFY_OK if the device opened successfully; otherwise,
 *         another appropriate error code.
 */
int cueify_device_close_unportable(cueify_device_private *d);


/** Unportable version of cueify_device_get_default_device().
 *
 * @return the OS-specific device identifier of the default optical
 *         disc (CD-ROM) device, or NULL if none is available
 */
const char *cueify_device_get_default_device_unportable();

#endif  /* _LIBCUEIFY_DEVICE_PRIVATE_H */
