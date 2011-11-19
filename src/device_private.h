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

#include <libcueify/types.h>

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

#define RAW_SECTOR_SIZE  2352  /** Number of bytes in a raw CD sector. */

/** Format of data returned from a raw CD read. */
typedef struct {
    uint8_t reserved1[0xF];  /** Header data before the data mode. */
    uint8_t data_mode;  /** The data mode flag. */
    /** Sector data after the data mode. */
    uint8_t reserved2[RAW_SECTOR_SIZE - 0x10];
#if defined(__FreeBSD__) || defined(_WIN32)
    /*
     * Subchannel Q data used to construct current position from READ
     * CD call. All values in binary-coded decimal.
     */
    uint8_t control_adr;  /** Control data in bits 7-4, ADR data in bits 3-0 */
    uint8_t track;  /** Track number of the current position */
    uint8_t index;  /** Index number of the current position */
    uint8_t min;  /** Minute offset of the current position relative to track */
    uint8_t sec;  /** Second offset of the current position relative to track */
    uint8_t frm;  /** Frame offset of the current position relative to track */
    uint8_t zero;  /** Zero */
    uint8_t amin;  /** Minute offset of the absolute position */
    uint8_t asec;  /** Second offset of the absolute position */
    uint8_t afrm;  /** Frame offset of the absolute position */
    uint8_t padding[6];  /** Padding (maybe some other irrelevant bits) */
#endif
} cueify_raw_read_private;

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


/** Unportable read of a raw sector from a disc in an optical disc drive.
 *
 * @note This should always return a complete 2352-byte raw sector
 *       regardless of the mode of the track.  In practice, this may
 *       be accomplished by invoking the MMC-3 READ CD call with the
 *       sync bits, all headers, user data, and ECC bits.  Some OS's
 *       (e.g. FreeBSD) may also support reading 16 extra bytes for
 *       data from subchannel Q, which is used to construct the
 *       "current position" of the track.
 *
 * @param d the cueify device handle to read from
 * @param lba the absolute address (LBA) of the sector to read
 * @param buffer the buffer of data to read the raw sector into
 * @return CUEIFY_OK if the read succeeded; otherwise, an appropriate error code.
 */
int cueify_device_read_raw_unportable(cueify_device_private *d, uint32_t lba,
				      cueify_raw_read_private *buffer);


/** Unportable version of cueify_device_close().
 *
 * @param d the cueify device handle to close
 * @return CUEIFY_OK if the device opened successfully; otherwise,
 *         another appropriate error code.
 */
int cueify_device_close_unportable(cueify_device_private *d);


/** Unportable version of cueify_device_get_supported_apis().
 *
 * @param d the cueify device handle for which to get supported APIs
 * @return a bitmask of the supported APIs of the device
 */
int cueify_device_get_supported_apis_unportable(cueify_device_private *d);


/** Unportable version of cueify_device_get_default_device().
 *
 * @return the OS-specific device identifier of the default optical
 *         disc (CD-ROM) device, or NULL if none is available
 */
const char *cueify_device_get_default_device_unportable();

#endif  /* _LIBCUEIFY_DEVICE_PRIVATE_H */
