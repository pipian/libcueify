/* darwin.c - Darwin-specific (OS X) CD-ROM API glue
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

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>
#include <libcueify/toc.h>
#include <libcueify/sessions.h>
#include <libcueify/full_toc.h>
#include <libcueify/cdtext.h>
#include <libcueify/indices.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "toc_private.h"
#include "sessions_private.h"
#include "full_toc_private.h"
#include "cdtext_private.h"
#include "indices_private.h"

int cueify_device_open_unportable(cueify_device_private *d,
				  const char *device) {
    int fd;

    /* TODO: Test if the file is actually a CD-ROM device. */
    fd = open(device, O_RDONLY, 0);
    if (fd < 0) {
	return CUEIFY_ERR_NO_DEVICE;
    }
    d->handle = fd;
    return CUEIFY_OK;
}  /* cueify_device_open_unportable */


int cueify_device_close_unportable(cueify_device_private *d) {
    if (close(d->handle) == 0) {
	return CUEIFY_OK;
    } else {
	return CUEIFY_ERR_INTERNAL;
    }
}  /* cueify_device_close_unportable */


int cueify_device_get_supported_apis_unportable(cueify_device_private *d) {
    /* Suppress error about d. */
    d++;
    return (CUEIFY_DEVICE_SUPPORTS_TOC);
}  /* cueify_device_get_supported_apis_unportable */


const char *cueify_device_get_default_device_unportable() {
    CFDictionaryRef matching;
    io_service_t service;
    DADiskRef disk;
    DASessionRef session;
    /* NOTE: Not thread-safe :( */
    static char device[1024] = "/dev/r";

    matching = IOServiceMatching(kIOCDMediaClass);
    service = IOServiceGetMatchingService(kIOMasterPortDefault, matching);
    session = DASessionCreate(kCFAllocatorDefault);
    disk = DADiskCreateFromIOMedia(kCFAllocatorDefault, session, service);
    if (disk == NULL) {
	/* No such disk exists. */
	CFRelease(session);
	IOObjectRelease(service);
	return NULL;
    }
    strncpy(device + 6, DADiskGetBSDName(disk), 1023 - 6);
    device[1023] = '\0';
    CFRelease(disk);
    CFRelease(session);
    IOObjectRelease(service);

    return device;
}  /* cueify_device_get_default_device_unportable */


int cueify_device_read_toc_unportable(cueify_device_private *d,
				      cueify_toc_private *t) {
    uint8_t data[4 + MAX_TRACKS * 8];
    uint16_t size = 4 + MAX_TRACKS * 8;
    dk_cd_read_toc_t toc;

    memset(data, 0, size);
    memset(&toc, 0, sizeof(toc));
    toc.format = 0x00;  /* 0000b = TOC */
    toc.formatAsTime = 0;  /* 0 = LBA */
    toc.bufferLength = size;
    toc.buffer = data;

    if (ioctl(d->handle, DKIOCCDREADTOC, &toc) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    return cueify_toc_deserialize((cueify_toc *)t, toc.buffer,
				  toc.bufferLength);
}  /* cueify_device_read_toc_unportable */


int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s) {
    uint8_t data[12];
    uint16_t size = 12;
    dk_cd_read_toc_t toc;

    memset(data, 0, size);
    memset(&toc, 0, sizeof(toc));
    toc.format = 0x01;  /* 0001b = Session Info */
    toc.formatAsTime = 0;  /* 0 = LBA */
    toc.bufferLength = size;
    toc.buffer = data;

    if (ioctl(d->handle, DKIOCCDREADTOC, &toc) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    return cueify_sessions_deserialize((cueify_sessions *)s, toc.buffer,
				       toc.bufferLength);
}  /* cueify_device_read_sessions_unportable */


int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t) {
    uint8_t data[256 * 11 + 4];
    uint16_t size = 256 * 11 + 4;
    dk_cd_read_toc_t toc;

    memset(data, 0, size);
    memset(&toc, 0, sizeof(toc));
    toc.format = kCDTOCFormatTOC;  /* Full TOC */
    toc.formatAsTime = 0;  /* 0 = LBA */
    toc.bufferLength = size;
    toc.buffer = data;

    if (ioctl(d->handle, DKIOCCDREADTOC, &toc) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    return cueify_full_toc_deserialize((cueify_full_toc *)t, toc.buffer,
				       toc.bufferLength);
}  /* cueify_device_read_full_toc_unportable */


int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t) {
    uint8_t data[256 * 11 + 4];
    uint16_t size = 256 * 11 + 4;
    dk_cd_read_toc_t toc;

    memset(data, 0, size);
    memset(&toc, 0, sizeof(toc));
    toc.format = kCDTOCFormatTEXT;  /* CD-Text */
    toc.formatAsTime = 0;  /* 0 = LBA */
    toc.bufferLength = size;
    toc.buffer = data;

    if (ioctl(d->handle, DKIOCCDREADTOC, &toc) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    return cueify_cdtext_deserialize((cueify_cdtext *)t, toc.buffer,
				     toc.bufferLength);
}  /* cueify_device_read_cdtext_unportable */


int cueify_device_read_mcn_unportable(cueify_device_private *d,
				      char *buffer, size_t *size) {
    d = 0;
    buffer = 0;
    size = 0;
    return CUEIFY_NO_DATA;
}  /* cueify_device_read_mcn_unportable */


int cueify_device_read_isrc_unportable(cueify_device_private *d, uint8_t track,
				       char *buffer, size_t *size) {
    d = 0;
    track = 0;
    buffer = 0;
    size = 0;
    return CUEIFY_NO_DATA;
}  /* cueify_device_read_isrc_unportable */


int cueify_device_read_position_unportable(cueify_device_private *d,
					   uint8_t track, uint32_t lba,
					   cueify_position_t *pos) {
    d = 0;
    track = 0;
    lba = 0;
    pos = 0;
    return CUEIFY_NO_DATA;
}  /* cueify_device_read_position_unportable */


int cueify_device_read_raw_unportable(cueify_device_private *d, uint32_t lba,
				      cueify_raw_read_private *buffer) {
    d = 0;
    lba = 0;
    buffer = 0;
    return CUEIFY_NO_DATA;
}  /* cueify_device_read_raw_unportable */
