/* linux.c - Linux-specific CD-ROM API glue
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
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <cueify/toc.h>
#include <cueify/sessions.h>
#include <cueify/full_toc.h>
#include <cueify/cdtext.h>
#include <cueify/track_data.h>
#include <cueify/error.h>
#include "device_private.h"
#include "toc_private.h"
#include "sessions_private.h"
#include "full_toc_private.h"
#include "cdtext_private.h"
#include "indices_private.h"

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

/* Type of subchannel value to read with scsi_read_subchannel. */
#define IOCTL_CDROM_SUB_Q_CHANNEL         0x00
#define IOCTL_CDROM_CURRENT_POSITION      0x01
#define IOCTL_CDROM_MEDIA_CATALOG         0x02
#define IOCTL_CDROM_TRACK_ISRC            0x03

/** Struct representing READ SUB-CHANNEL command structure */
struct scsi_read_subchannel {
    uint8_t op_code;      /** MMC op code (0x42) */
    uint8_t time_fmt;     /** Time format in bit 1 (0x02=MSF, 0x00=LBA) */
    uint8_t read_subq;    /** Read the Q sub-channel data if bit 6 set (0x40) */
    uint8_t type;         /** Type of subchannel value to read */
    uint8_t reserved[2];
    uint8_t track;        /** Track number to read ISRC from */
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

/** Struct representing header of READ SUB-CHANNEL response */
struct subchannel_hdr {
    uint8_t reserved;
    uint8_t audio_status;
    uint8_t data_len[2];
};

/**
 * Struct representing body of READ SUB-CHANNEL response for
 * IOCTL_CDROM_MEDIA_CATALOG
 */
struct subchannel_mcn {
    struct subchannel_hdr header;
    uint8_t format_code;
    uint8_t reserved[3];
    uint8_t mcval;  /** If bit 7 is set, mcn is a valid media catalog number. */
    uint8_t mcn[15];
};

struct subchannel_isrc {
    struct subchannel_hdr header;
    uint8_t format_code;
    uint8_t reserved0;
    uint8_t track;
    uint8_t reserved1;
    uint8_t tcval;  /** If bit 7 is set, isrc is a valid ISRC. */
    uint8_t isrc[13];
    uint8_t reserved2[2];
};

struct subchannel_position {
    struct subchannel_hdr header;
    uint8_t format_code;
    uint8_t ctrl_adr;  /* ADR in 4 MSBs, Ctrl in 4 LSBs. */
    uint8_t track;
    uint8_t index;
    uint8_t abs_addr[4];
    uint8_t rel_addr[4];
};
 
union subchannel_data {
    struct subchannel_position position;
    struct subchannel_mcn mcn;
    struct subchannel_isrc isrc;
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


int cueify_device_get_supported_apis_unportable(cueify_device_private *d) {
    /* Suppress error about d. */
    d++;
    return (CUEIFY_DEVICE_SUPPORTS_TOC       |
	    CUEIFY_DEVICE_SUPPORTS_SESSIONS  |
	    CUEIFY_DEVICE_SUPPORTS_FULL_TOC  |
	    CUEIFY_DEVICE_SUPPORTS_CDTEXT    |
	    CUEIFY_DEVICE_SUPPORTS_MCN_ISRC  |
	    CUEIFY_DEVICE_SUPPORTS_INDICES   |
	    CUEIFY_DEVICE_SUPPORTS_DATA_MODE |
	    CUEIFY_DEVICE_SUPPORTS_TRACK_CONTROL);
}  /* cueify_device_get_supported_apis_unportable */


const char *cueify_device_get_default_device_unportable() {
    struct stat buf;

    /* Test for /dev/scd0, /dev/cdrom, and /dev/dvdrom */
    if (stat("/dev/scd0", &buf) == 0) {
	return "/dev/scd0";
    } else if (stat("/dev/cdrom", &buf) == 0) {
	return "/dev/cdrom";
    } else if (stat("/dev/dvdrom", &buf) == 0) {
	return "/dev/dvdrom";
    } else {
	return NULL;
    }
}  /* cueify_device_get_default_device_unportable */


int cueify_device_read_toc_unportable(cueify_device_private *d,
				      cueify_toc_private *t) {
    struct cdrom_tochdr hdr;
    struct cdrom_tocentry toc;
    int i;

    memset(&hdr, 0, sizeof(hdr));
    memset(&toc, 0, sizeof(toc));

    if (ioctl(d->handle, CDROMREADTOCHDR, &hdr) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    t->first_track_number = hdr.cdth_trk0;
    t->last_track_number = hdr.cdth_trk1;

    toc.cdte_format = CDROM_LBA;

    for (i = 0; i < MAX_TRACKS; i++) {
	toc.cdte_track = i + 1;
	if (i == MAX_TRACKS - 1) {
	    toc.cdte_track = 0xAA;
	}

	if (ioctl(d->handle, CDROMREADTOCENTRY, &toc) < 0) {
	    return CUEIFY_ERR_INTERNAL;
	}

	if (toc.cdte_track == 0xAA) {
	    /* Lead-out Track */
	    t->tracks[0].control = toc.cdte_ctrl;
	    t->tracks[0].adr = toc.cdte_adr;
	    t->tracks[0].lba = toc.cdte_addr.lba;
	} else if (toc.cdte_track != 0) {
	    t->tracks[toc.cdte_track].control = toc.cdte_ctrl;
	    t->tracks[toc.cdte_track].adr = toc.cdte_adr;
	    t->tracks[toc.cdte_track].lba = toc.cdte_addr.lba;
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_toc_unportable */


int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s) {
    /* CDROMMULTISESSION ioctl gives too little info. */
    struct cdrom_generic_command gpcmd;
    struct scsi_read_toc *scsi_cmd;
    struct request_sense sense;
    uint8_t data[12];

    memset(&gpcmd, 0, sizeof(gpcmd));

    scsi_cmd = (struct scsi_read_toc *)&gpcmd.cmd;
    scsi_cmd->format |= 0x01;  /* 0001b = Session Info */
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;
    scsi_cmd->op_code = GPCMD_READ_TOC_PMA_ATIP;

    gpcmd.buffer = data;
    gpcmd.buflen = sizeof(data);
    gpcmd.sense = &sense;
    gpcmd.data_direction = CGC_DATA_READ;
    gpcmd.timeout = 50000;

    if (ioctl(d->handle, CDROM_SEND_PACKET, &gpcmd) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* We serialize to the format of the TOC response for a reason... */
    return cueify_sessions_deserialize((cueify_sessions *)s,
				       data, sizeof(data));
}  /* cueify_device_read_sessions_unportable */


int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t) {
    struct cdrom_generic_command gpcmd;
    struct scsi_read_toc *scsi_cmd;
    struct request_sense sense;
    uint8_t data[256 * 11 + 4];

    memset(&gpcmd, 0, sizeof(gpcmd));

    scsi_cmd = (struct scsi_read_toc *)&gpcmd.cmd;
    scsi_cmd->format |= 0x02;  /* 0010b = Full TOC */
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;
    scsi_cmd->op_code = GPCMD_READ_TOC_PMA_ATIP;

    gpcmd.buffer = data;
    gpcmd.buflen = sizeof(data);
    gpcmd.sense = &sense;
    gpcmd.data_direction = CGC_DATA_READ;
    gpcmd.timeout = 50000;

    if (ioctl(d->handle, CDROM_SEND_PACKET, &gpcmd) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* We serialize to the format of the TOC response for a reason... */
    return cueify_full_toc_deserialize((cueify_full_toc *)t,
				       data, sizeof(data));
}  /* cueify_device_read_full_toc_unportable */


int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t) {
    struct cdrom_generic_command gpcmd;
    struct scsi_read_toc *scsi_cmd;
    struct request_sense sense;
    /* At most 2048 descriptors at 18 bytes a piece, plus 4 bytes header. */
    uint8_t data[2048 * 18 + 4];

    memset(&gpcmd, 0, sizeof(gpcmd));

    scsi_cmd = (struct scsi_read_toc *)&gpcmd.cmd;
    scsi_cmd->format |= 0x05;  /* 0101b = CD-TEXT */
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;
    scsi_cmd->op_code = GPCMD_READ_TOC_PMA_ATIP;

    gpcmd.buffer = data;
    gpcmd.buflen = sizeof(data);
    gpcmd.sense = &sense;
    gpcmd.data_direction = CGC_DATA_READ;
    gpcmd.timeout = 50000;

    if (ioctl(d->handle, CDROM_SEND_PACKET, &gpcmd) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* We serialize to the format of the TOC response for a reason... */
    return cueify_cdtext_deserialize((cueify_cdtext *)t,
				     data, sizeof(data));
}  /* cueify_device_read_cdtext_unportable */


int cueify_device_read_mcn_unportable(cueify_device_private *d,
				      char *buffer, size_t *size) {
    struct cdrom_mcn mcn;

    memset(&mcn, 0, sizeof(mcn));

    if (ioctl(d->handle, CDROM_GET_MCN, &mcn) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    *size = min(sizeof(mcn.medium_catalog_number), *size);
    memcpy(buffer, mcn.medium_catalog_number, *size - 1);
    buffer[*size] = '\0';
    if (buffer[0] == '\0') {
	*size = 1;
	return CUEIFY_NO_DATA;
    }

    return CUEIFY_OK;
}  /* cueify_device_read_mcn_unportable */


int cueify_device_read_isrc_unportable(cueify_device_private *d, uint8_t track,
				       char *buffer, size_t *size) {
    struct cdrom_generic_command gpcmd;
    struct scsi_read_subchannel *scsi_cmd;
    struct request_sense sense;
    union subchannel_data data;

    memset(&gpcmd, 0, sizeof(gpcmd));

    scsi_cmd = (struct scsi_read_subchannel *)&gpcmd.cmd;
    scsi_cmd->read_subq |= 0x40;  /* SUBQ = 1 */
    scsi_cmd->type = IOCTL_CDROM_TRACK_ISRC;
    scsi_cmd->track = track;
    scsi_cmd->data_len[0] = (sizeof(data) >> 8) & 0xFF;
    scsi_cmd->data_len[1] = sizeof(data) & 0xFF;
    scsi_cmd->op_code = GPCMD_READ_SUBCHANNEL;

    gpcmd.buffer = (unsigned char *)&data;
    gpcmd.buflen = sizeof(data);
    gpcmd.sense = &sense;
    gpcmd.data_direction = CGC_DATA_READ;
    gpcmd.timeout = 50000;

    if (ioctl(d->handle, CDROM_SEND_PACKET, &gpcmd) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    /* TCVAL (MSB in byte 8) must equal 1 if there is ISRC data. */
    if (data.isrc.tcval != 0x80) {
	/* No data. */
	if (*size > 0) {
	    *size = 1;
	    buffer[0] = '\0';
	}
	return CUEIFY_NO_DATA;
    }

    /* OK.  Copy the ISRC (starts at byte 9) */
    *size = min(sizeof(data.isrc.isrc), *size);
    if (*size > 0) {
	memcpy(buffer, data.isrc.isrc, *size - 1);
	buffer[*size - 1] = '\0';
    }

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
     * Manual testing shows that, on SOME discs, we will return all
     * zeroes for the Q subchannel!
     */
    while (buffer.track == 0) {
	/*
	 * We can actually get the position from reading the Q subchannel
	 * during our raw read, rather than doing a subchannel ioctl!
	 */
	if (cueify_device_read_raw_unportable(d, lba, &buffer) != CUEIFY_OK) {
	    return CUEIFY_ERR_INTERNAL;
	}
	lba--;
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
    struct cdrom_generic_command gpcmd;
    struct scsi_read_cd *scsi_cmd;
    struct request_sense sense;

    memset(&gpcmd, 0, sizeof(gpcmd));

    scsi_cmd = (struct scsi_read_cd *)&gpcmd.cmd;
    scsi_cmd->address[0] = (lba >> 24);
    scsi_cmd->address[1] = (lba >> 16) & 0xFF;
    scsi_cmd->address[2] = (lba >> 8) & 0xFF;
    scsi_cmd->address[3] = lba & 0xFF;

    scsi_cmd->length[0] = 0;
    scsi_cmd->length[1] = 0;
    scsi_cmd->length[2] = 1;

    scsi_cmd->bitmask = 0xF8;  /* Read Sync bit, all headers, User Data, and ECC */
    scsi_cmd->subchannels = 0x02;  /* Support the Q subchannel */

    scsi_cmd->op_code = GPCMD_READ_CD;

    gpcmd.buffer = (unsigned char *)buffer;
    gpcmd.buflen = sizeof(buffer);
    gpcmd.sense = &sense;
    gpcmd.data_direction = CGC_DATA_READ;
    gpcmd.timeout = 50000;

    if (ioctl(d->handle, CDROM_SEND_PACKET, &gpcmd) < 0) {
	return CUEIFY_ERR_INTERNAL;
    }

    return CUEIFY_OK;
}  /* cueify_device_read_raw_unportable */
