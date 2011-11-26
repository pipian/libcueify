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
 * Create a new handle for an optical disc (CD-ROM) device.
 *
 * @return NULL if there was an error allocating memory, else the new handle
 */
cueify_device *cueify_device_new();


/**
 * Open an optical disc (CD-ROM) device and associate it with a device
 * handle.
 *
 * This function should be called after cueify_device_new() but before
 * any other cueify_device_*() functions.
 *
 * @pre { d != NULL }
 * @param d an unopened device handle
 * @param device an operating-system-specific device identifier of the
 *               device to open, or NULL to open the default device
 *               returned by cueify_device_get_default_device().
 * @return CUEIFY_OK if the device was successfully opened, otherwise,
 *         an error code is returned. This error code may include:
 *         CUEIFY_ERR_NO_DEVICE if the device specified by device
 *         could not be found, or device is NULL and
 *         cueify_get_default_device() returns NULL.
 */
int cueify_device_open(cueify_device *d, const char *device);


/**
 * Close the optical disc device associated with a device handle.
 *
 * @note The device handle may be reused with cueify_device_open()
 *       once it has been closed successfully.
 *
 * @pre { d != NULL, cueify_device_open(d) last returned CUEIFY_OK }
 * @param d an opened device handle
 * @return CUEIFY_OK if the device was successfully closed; otherwise
 *         an error code is returned
 */
int cueify_device_close(cueify_device *d);


/**
 * Free a handle for an optical disc (CD-ROM) device. Deletes the
 * object pointed to by d.
 *
 * @pre { d != NULL }
 * @param d a cueify_device object created by cueify_device_new()
 */
void cueify_device_free(cueify_device *d);


/** Device supports reading TOC */
#define CUEIFY_DEVICE_SUPPORTS_TOC        0x01
/** Device supports reading multi-session data */
#define CUEIFY_DEVICE_SUPPORTS_SESSIONS   0x02
/** Device supports reading the full TOC */
#define CUEIFY_DEVICE_SUPPORTS_FULL_TOC   0x04
/** Device supports reading CD-Text */
#define CUEIFY_DEVICE_SUPPORTS_CDTEXT     0x08
/**
 * Device supports reading Media Catalog Numbers/International
 * Standard Recording Codes (ISRCs).
 */
#define CUEIFY_DEVICE_SUPPORTS_MCN_ISRC   0x10
/** Device supports reading track indices */
#define CUEIFY_DEVICE_SUPPORTS_INDICES    0x20
/** Device supports reading data modes */
#define CUEIFY_DEVICE_SUPPORTS_DATA_MODE  0x40


/**
 * Return a bitmask of libcueify APIs this device supports on this
 * operating system.
 *
 * @note The returned bitmask can only guarantee that unsupported APIs
 * do not work.  Certain APIs cannot be determined to be supported
 * without testing against a disk known to return data for them.
 *
 * @param d the device for which to return API support
 * @return a bitmask of supported APIs
 */
int cueify_device_get_supported_apis(cueify_device *d);


/**
 * Get an operating-system-specific device identifier for the default
 * optical disc (CD-ROM) device in this system.
 *
 * @note This function is not guaranteed to be thread-safe (e.g. on Darwin)
 *
 * @return NULL if there is no default optical disc (CD-ROM) device in
 *         this system, else the identifier of the default device
 */
const char *cueify_device_get_default_device();

#endif /* _LIBCUEIFY_DEVICE_H */
