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

#define READ_TOC  0x43  /** MMC op code for READ TOC/PMA/ATIP */
#define READ_CD   0xBE  /** MMC op code for READ CD */

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

/** Struct representing READ CD command structure */
struct scsi_read_cd {
    uint8_t op_code;      /** MMC op code (0xBE) */
    uint8_t sector_type;  /** Expected sector type in bits 2-4 */
    uint8_t address[4];   /** Starting logical block address */
    uint8_t length[3];    /** Transfer length */
    /**
     * Sync bit in bit 7, Header Code in bits 6-5, User Data bit in bit 4,
     * EDC/ECC bit in bit 3, Erro field in bits 2-1
     */
    uint8_t bitmask;
    uint8_t subchannels;  /** Subchannel selection bits in bits 2-0 */
    uint8_t control;      /** Control data */
};

#define min(x, y)  ((x > y) ? y : x)  /** Return the minimum of x and y. */

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


int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t) {
    char link_path[1024];
    struct cam_device *camdev;
    union ccb *ccb;
    struct ccb_scsiio *csio;
    /* At most 2048 descriptors at 18 bytes a piece, plus 4 bytes header. */
    uint8_t data[2048 * 18 + 4];
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

    scsi_cmd->format |= 0x05;  /* 0101b = CD-TEXT */
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
    if (cueify_cdtext_deserialize((cueify_cdtext *)t,
				    data, sizeof(data)) != CUEIFY_OK) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    cam_freeccb(ccb);
    cam_close_device(camdev);

    return CUEIFY_OK;
}  /* cueify_device_read_cdtext_unportable */


int cueify_device_read_mcn_unportable(cueify_device_private *d,
				      char *buffer, size_t *size) {
    struct ioc_read_subchannel subchannel;
    struct cd_sub_channel_info info;

    memset(&info, 0, sizeof(info));

    subchannel.address_format = CD_MSF_FORMAT;
    subchannel.data_format = CD_MEDIA_CATALOG;
    subchannel.track = 0;
    subchannel.data_len = sizeof(info);
    subchannel.data = &info;

    if (ioctl(d->handle, CDIOCREADSUBCHANNEL, &subchannel) < 0) {
	return CUEIFY_ERR_INTERNAL;
    } else if (!info.what.media_catalog.mc_valid) {
	return CUEIFY_NO_DATA;
    }

    *size = min(sizeof(info.what.media_catalog.mc_number) + 1, *size);
    memcpy(buffer, info.what.media_catalog.mc_number, *size - 1);
    buffer[*size - 1] = '\0';

    return CUEIFY_OK;
}  /* cueify_device_read_mcn_unportable */


int cueify_device_read_isrc_unportable(cueify_device_private *d, uint8_t track,
				       char *buffer, size_t *size) {
    struct ioc_read_subchannel subchannel;
    struct cd_sub_channel_info info;

    memset(&info, 0, sizeof(info));

    subchannel.address_format = CD_MSF_FORMAT;
    subchannel.data_format = CD_TRACK_INFO;
    subchannel.track = track;
    subchannel.data_len = sizeof(info);
    subchannel.data = &info;

    if (ioctl(d->handle, CDIOCREADSUBCHANNEL, &subchannel) < 0) {
	return CUEIFY_ERR_INTERNAL;
    } else if (!info.what.track_info.ti_valid) {
	return CUEIFY_NO_DATA;
    }

    *size = min(sizeof(info.what.track_info.ti_number) + 1, *size);
    memcpy(buffer, info.what.track_info.ti_number, *size - 1);
    buffer[*size - 1] = '\0';

    return CUEIFY_OK;
}  /* cueify_device_read_isrc_unportable */


/** Return the binary representation of a binary-coded decimal. */
#define BCD2BIN(x)  (((x >> 4) & 0xF) * 10 + (x & 0xF))


int cueify_device_read_position_unportable(cueify_device_private *d,
					   uint8_t track, uint32_t lba,
					   cueify_position_t *pos) {
    cueify_raw_read_private buffer;

    /* Do nothing, but remove error where track is unused! */
    buffer.data_mode = track;
    memset(&buffer, 0, sizeof(buffer));

    /*
     * We can actually get the position from reading the Q subchannel
     * during our raw read, rather than doing a subchannel ioctl!
     */
    if (cueify_device_read_raw_unportable(d, lba, &buffer) != CUEIFY_OK) {
	return CUEIFY_ERR_INTERNAL;
    }

    pos->track = BCD2BIN(buffer.track);
    pos->index = BCD2BIN(buffer.index);

    /* Times are given in binary-coded decimal. */
    pos->abs.min = BCD2BIN(buffer.amin);
    pos->abs.sec = BCD2BIN(buffer.asec);
    pos->abs.frm = BCD2BIN(buffer.afrm);

    /* Adjust the absolute time by 2 seconds for the lead-in. */
    if (pos->abs.sec < 2) {
	pos->abs.sec += 75;
	pos->abs.min--;
    }
    pos->abs.sec -= 2;

    /* I expect that relative times are in BCD as well. */
    pos->rel.min = BCD2BIN(buffer.min);
    pos->rel.sec = BCD2BIN(buffer.sec);
    pos->rel.frm = BCD2BIN(buffer.frm);

    return CUEIFY_OK;
}  /* cueify_device_read_position_unportable */


int cueify_device_read_raw_unportable(cueify_device_private *d, uint32_t lba,
				      cueify_raw_read_private *buffer) {
    char link_path[1024];
    struct cam_device *camdev;
    union ccb *ccb;
    struct ccb_scsiio *csio;
    struct scsi_read_cd *scsi_cmd;
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
		  /* data_ptr */ (unsigned char *)&buffer,
		  /* dxfer_len */ sizeof(cueify_raw_read_private),
		  /* sense_len */ SSD_FULL_SIZE,
		  sizeof(struct scsi_read_cd),
		  /* timeout */ 50000);

    scsi_cmd = (struct scsi_read_cd *)&csio->cdb_io.cdb_bytes;
    bzero(scsi_cmd, sizeof(*scsi_cmd));

    scsi_cmd->address[0] = (lba >> 24);
    scsi_cmd->address[1] = (lba >> 16) & 0xFF;
    scsi_cmd->address[2] = (lba >> 8) & 0xFF;
    scsi_cmd->address[3] = lba & 0xFF;

    scsi_cmd->length[0] = 0;
    scsi_cmd->length[1] = 0;
    scsi_cmd->length[2] = 1;

    scsi_cmd->bitmask = 0xF8;  /* Read Sync bit, all headers, User Data, and ECC */
    scsi_cmd->subchannels = 0x02;  /* Support the Q subchannel */

    scsi_cmd->op_code = READ_CD;

    result = cam_send_ccb(camdev, ccb);
    if (result < 0) {
	cam_freeccb(ccb);
	cam_close_device(camdev);
	return CUEIFY_ERR_INTERNAL;
    }

    cam_freeccb(ccb);
    cam_close_device(camdev);

    return CUEIFY_OK;
}  /* cueify_device_read_raw_unportable */
