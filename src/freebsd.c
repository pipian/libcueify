/* freebsd.c - FreeBSD-specific CD-ROM API glue
 *
 * Copyright (c) 2011 Ian Jacobi
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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <camlib.h>
#include <cam/scsi/scsi_message.h>
#include <sys/ioctl.h>
#include <sys/cdio.h>
#include <libcueify/sessions.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "toc_private.h"
#include "sessions_private.h"

#define READ_TOC  0x43  /** MMC op code for READ TOC/PMA/ATIP */

/** Struct representing READ TOC/PMA/ATIP command structure */
struct scsi_read_toc {
    uint8_t op_code;      /** MMC op code (0x43) */
    uint8_t time_fmt;     /** Time format in bit 1 (0x02=MSF, 0x00=LBA) */
    uint8_t format;       /** TOC format to return */
    uint8_t unused[3];    /** Unused bytes */
    uint8_t track;        /** Track number to fetch data for */
    uint8_t data_len[2];  /** Size of the structure to fill */
    uint8_t control;      /** Control data */
};

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


const char *cueify_device_get_default_device_unportable() {
    struct stat buf;

    /* Test for /dev/cd0 and /dev/acd0 */
    if (stat("/dev/cd0", &buf) == 0) {
	return "/dev/cd0";
    } else if (stat("/dev/acd0", &buf) == 0) {
	return "/dev/acd0";
    } else {
	return NULL;
    }
}  /* cueify_device_get_default_device_unportable */


int cueify_device_read_toc_unportable(cueify_device_private *d,
				      cueify_toc_private *t) {
    struct ioc_toc_header hdr;
    struct cd_toc_entry entries[100];
    struct ioc_read_toc_entry toc;
    int i;

    memset(entries, 0, sizeof(entries));

    if (ioctl(d->handle, CDIOREADTOCHEADER, &hdr) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    t->first_track_number = hdr.starting_track;
    t->last_track_number = hdr.ending_track;

    toc.address_format = CD_LBA_FORMAT;
    toc.starting_track = 0;
    toc.data_len = sizeof(entries);
    toc.data = entries;

    if (ioctl(d->handle, CDIOREADTOCENTRYS, &toc) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    for (i = 0; i < MAX_TRACKS; i++) {
	if (toc.data[i].track == 0xAA) {
	    /* Lead-out Track */
	    t->tracks[0].control = toc.data[i].control;
	    t->tracks[0].adr = toc.data[i].addr_type;
	    t->tracks[0].lba =
		((toc.data[i].addr.addr[0] << 24) |
		 (toc.data[i].addr.addr[1] << 16) |
		 (toc.data[i].addr.addr[2] << 8) |
		 toc.data[i].addr.addr[3]);
	} else if (toc.data[i].track != 0) {
	    t->tracks[toc.data[i].track].control = toc.data[i].control;
	    t->tracks[toc.data[i].track].adr = toc.data[i].addr_type;
	    t->tracks[toc.data[i].track].lba =
		((toc.data[i].addr.addr[0] << 24) |
		 (toc.data[i].addr.addr[1] << 16) |
		 (toc.data[i].addr.addr[2] << 8) |
		 toc.data[i].addr.addr[3]);
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_toc_unportable */


int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s) {
    char link_path[1024];
    struct cam_device *camdev;
    union ccb *ccb;
    struct ccb_scsiio *csio;
    uint8_t data[12];
    struct scsi_read_toc *scsi_cmd;
    int result;

    /* cam_open_device does not resolve symlinks. */
    memset(link_path, 0, sizeof(link_path));
    result = readlink(d->path, link_path, sizeof(link_path) - 1);
    if (result < 0 && errno != EINVAL) {
	return CUEIFY_ERR_INTERNAL;
    } else if (result < 0) {  /* errno == EINVAL */
	camdev = cam_open_device(d->path, O_RDWR);
    } else {
	camdev = cam_open_device(link_path, O_RDWR);
    }

    if (camdev == NULL) {
	return CUEIFY_ERR_INTERNAL;
    }

    ccb = cam_getccb(camdev);
    if (ccb == NULL) {
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    csio = &ccb->csio;
    cam_fill_csio(csio,
		  /* retries */ 4,
		  /* cbfcnp */ NULL,
		  /* flags */ CAM_DIR_IN,
		  /* tag_action */ MSG_SIMPLE_Q_TAG,
		  /* data_ptr */ data,
		  /* dxfer_len */ sizeof(data),
		  /* sense_len */ SSD_FULL_SIZE,
		  sizeof(struct scsi_read_toc),
		  /* timeout */ 50000);

    scsi_cmd = (struct scsi_read_toc *)&csio->cdb_io.cdb_bytes;
    bzero(scsi_cmd, sizeof(*scsi_cmd));

    scsi_cmd->format |= 0x01;  /* 0001b = Session Info */
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;

    scsi_cmd->op_code = READ_TOC;

    result = cam_send_ccb(camdev, ccb);
    if (result < 0) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    /* We serialize to the format of the TOC response for a reason... */
    if (cueify_sessions_deserialize((cueify_sessions *)s,
				    data, sizeof(data)) != CUEIFY_OK) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    cam_freeccb(ccb);
    cam_close_device(camdev);

    return CUEIFY_OK;
}  /* cueify_device_read_sessions_unportable */


int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t) {
    char link_path[1024];
    struct cam_device *camdev;
    union ccb *ccb;
    struct ccb_scsiio *csio;
    uint8_t data[256 * 11 + 4];
    struct scsi_read_toc *scsi_cmd;
    int result;

    /* cam_open_device does not resolve symlinks. */
    memset(link_path, 0, sizeof(link_path));
    result = readlink(d->path, link_path, sizeof(link_path) - 1);
    if (result < 0 && errno != EINVAL) {
	return CUEIFY_ERR_INTERNAL;
    } else if (result < 0) {  /* errno == EINVAL */
	camdev = cam_open_device(d->path, O_RDWR);
    } else {
	camdev = cam_open_device(link_path, O_RDWR);
    }

    if (camdev == NULL) {
	return CUEIFY_ERR_INTERNAL;
    }

    ccb = cam_getccb(camdev);
    if (ccb == NULL) {
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    csio = &ccb->csio;
    cam_fill_csio(csio,
		  /* retries */ 4,
		  /* cbfcnp */ NULL,
		  /* flags */ CAM_DIR_IN,
		  /* tag_action */ MSG_SIMPLE_Q_TAG,
		  /* data_ptr */ data,
		  /* dxfer_len */ sizeof(data),
		  /* sense_len */ SSD_FULL_SIZE,
		  sizeof(struct scsi_read_toc),
		  /* timeout */ 50000);

    scsi_cmd = (struct scsi_read_toc *)&csio->cdb_io.cdb_bytes;
    bzero(scsi_cmd, sizeof(*scsi_cmd));

    scsi_cmd->format |= 0x02;  /* 0010b = Full TOC */
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;

    scsi_cmd->op_code = READ_TOC;

    result = cam_send_ccb(camdev, ccb);
    if (result < 0) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    /* We serialize to the format of the TOC response for a reason... */
    if (cueify_full_toc_deserialize((cueify_full_toc *)t,
				    data, sizeof(data)) != CUEIFY_OK) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    cam_freeccb(ccb);
    cam_close_device(camdev);

    return CUEIFY_OK;
}  /* cueify_device_read_full_toc_unportable */
