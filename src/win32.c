/* win32.c - win32-specific (Windows XP+) CD-ROM API glue
 *
 * Copyright (c) 2010, 2011 Ian Jacobi <pipian@pipian.com>
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

#include <windows.h>
#include <cueify/error.h>
#include <cueify/cdtext.h>
#include <cueify/full_toc.h>
#include "device_private.h"
#include "toc_private.h"
#include "sessions_private.h"
#include "full_toc_private.h"
#include "cdtext_private.h"
#include "indices_private.h"
#include "win32.h"

#if defined(__WIN32__) && !defined(__CYGWIN__)
#include <tchar.h>
#else
typedef char TCHAR;
#endif

TCHAR szVolumeFormat[9] = "\\\\.\\%lc:";
TCHAR szRootFormat[6] = "%lc:\\";

int cueify_device_open_unportable(cueify_device_private *d,
				  const char *device) {
    HANDLE hVolume;
    WCHAR cDriveLetter;
    UINT uDriveType;
    TCHAR szVolumeName[7];
    TCHAR szRootName[4];
    DWORD dwAccessFlags;

    /* Precondition: device[0] != '\0' */

    /* Is it a drive letter? (Regular Expression: c(?::\\?)?) */
    if (device[1] == '\0' ||
	(device[1] == ':' &&
	 (device[2] == '\0' ||
	  (device[2] == '\\' && device[3] == '\0')))) {
	/* Just a single letter. */
	cDriveLetter = device[0];
    } else {
	/* We currently don't handle drives without a letter. */
	return CUEIFY_ERR_BADARG;
    }
    wsprintf(szRootName, szRootFormat, cDriveLetter);

    uDriveType = GetDriveType(szRootName);
    switch (uDriveType) {
    case DRIVE_REMOVABLE:
    case DRIVE_CDROM:
	dwAccessFlags = GENERIC_READ | GENERIC_WRITE;
	break;
    default:
	return CUEIFY_ERR_NO_DEVICE;
    }
    wsprintf(szVolumeName, szVolumeFormat, cDriveLetter);

    hVolume = CreateFile(szVolumeName,
			 dwAccessFlags,
			 FILE_SHARE_READ | FILE_SHARE_WRITE,
			 NULL,
			 OPEN_EXISTING,
			 0,
			 NULL);
    if (hVolume == INVALID_HANDLE_VALUE) {
	return CUEIFY_ERR_NO_DEVICE;
    } else {
	d->handle = hVolume;
	return CUEIFY_OK;
    }
}  /* cueify_device_open_unportable */

int cueify_device_close_unportable(cueify_device_private *d) {
    if (CloseHandle(d->handle)) {
	return CUEIFY_OK;
    } else {
	return CUEIFY_ERR_INTERNAL;
    }
}  /* cueify_device_close_unportable */


int cueify_device_get_supported_apis_unportable(cueify_device_private *d) {
    /* Suppress error about d. */
    d++;
    /*
     * Windows supports all APIs, but in the future we could try to
     * use GET CONFIGURATION to determine what the DEVICE can support.
     */
    return (CUEIFY_DEVICE_SUPPORTS_TOC       |
	    CUEIFY_DEVICE_SUPPORTS_SESSIONS  |
	    CUEIFY_DEVICE_SUPPORTS_FULL_TOC  |
	    CUEIFY_DEVICE_SUPPORTS_CDTEXT    |
	    CUEIFY_DEVICE_SUPPORTS_MCN_ISRC  |
	    CUEIFY_DEVICE_SUPPORTS_INDICES   |
	    CUEIFY_DEVICE_SUPPORTS_DATA_MODE |
	    CUEIFY_DEVICE_SUPPORTS_TRACK_CONTROL);
}  /* cueify_device_get_supported_apis_unportable */


const char *driveLetters[26] = {
    "a:\\", "b:\\", "c:\\", "d:\\", "e:\\", "f:\\", "g:\\", "h:\\",
    "i:\\", "j:\\", "k:\\", "l:\\", "m:\\", "n:\\", "o:\\", "p:\\",
    "q:\\", "r:\\", "s:\\", "t:\\", "u:\\", "v:\\", "w:\\", "x:\\",
    "y:\\", "z:\\"
};

const char *cueify_device_get_default_device_unportable() {
    int i;
    UINT uDriveType;
    DWORD dwDrives;
    TCHAR szRootName[4] = "a:\\";

    dwDrives = GetLogicalDrives();
    for (i = 0; i < 26; i++) {
	if (dwDrives & 0x01) {
	    uDriveType = GetDriveType(szRootName);
	    if (uDriveType == DRIVE_CDROM) {
		return driveLetters[i];
	    }
	}
	dwDrives >>= 1;
	szRootName[0]++;
    }
    return NULL;
}  /* cueify_device_get_default_device_unportable */

int cueify_device_read_toc_unportable(cueify_device_private *d,
				      cueify_toc_private *t) {
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    CDROM_TOC toc;
    BOOL succeeded;
    int i;
    
    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_TOC;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = FALSE;
    toc_ex.SessionTrack = 1;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;
    
    succeeded = DeviceIoControl(d->handle,
				IOCTL_CDROM_READ_TOC_EX,
				&toc_ex, sizeof(CDROM_READ_TOC_EX),
				&toc, sizeof(CDROM_TOC),
				&dwReturned, NULL);
    if (!succeeded) {
	return CUEIFY_ERR_INTERNAL;
    } else {
	t->first_track_number = toc.FirstTrack;
	t->last_track_number = toc.LastTrack;
	for (i = 0; i < MAXIMUM_NUMBER_TRACKS; i++) {
	    if (toc.TrackData[i].TrackNumber == 0xAA) {
		/* Lead-out Track */
		t->tracks[0].control = toc.TrackData[i].Control;
		t->tracks[0].adr = toc.TrackData[i].Adr;
		t->tracks[0].lba =
		    ((toc.TrackData[i].Address[0] << 24) |
		     (toc.TrackData[i].Address[1] << 16) |
		     (toc.TrackData[i].Address[2] << 8) |
		     toc.TrackData[i].Address[3]);
	    } else if (toc.TrackData[i].TrackNumber != 0) {
		t->tracks[toc.TrackData[i].TrackNumber].control =
		    toc.TrackData[i].Control;
		t->tracks[toc.TrackData[i].TrackNumber].adr =
		    toc.TrackData[i].Adr;
		t->tracks[toc.TrackData[i].TrackNumber].lba =
		    ((toc.TrackData[i].Address[0] << 24) |
		     (toc.TrackData[i].Address[1] << 16) |
		     (toc.TrackData[i].Address[2] << 8) |
		     toc.TrackData[i].Address[3]);
	    }
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_toc_unportable */


int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s) {
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    CDROM_TOC_SESSION_DATA session;
    BOOL succeeded;

    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_SESSION;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = FALSE;
    toc_ex.SessionTrack = 0;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;

    succeeded = DeviceIoControl(d->handle,
				IOCTL_CDROM_READ_TOC_EX,
				&toc_ex, sizeof(CDROM_READ_TOC_EX),
				&session, sizeof(CDROM_TOC_SESSION_DATA),
				&dwReturned, NULL);
    if (!succeeded) {
	return CUEIFY_ERR_INTERNAL;
    } else {
	s->first_session_number = session.FirstCompleteSession;
	s->last_session_number = session.LastCompleteSession;
	s->track_control = session.TrackData[0].Control;
	s->track_adr = session.TrackData[0].Adr;
	s->track_number = session.TrackData[0].TrackNumber;
	s->track_lba =
	  ((session.TrackData[0].Address[0] << 24) |
	   (session.TrackData[0].Address[1] << 16) |
	   (session.TrackData[0].Address[2] << 8) |
	   session.TrackData[0].Address[3]);
    }

    return CUEIFY_OK;
}  /* cueify_device_read_sessions_unportable */


int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t) {
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    int iSize = 256 * sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK) +
	sizeof(CDROM_TOC_FULL_TOC_DATA);
    uint8_t buf[256 * sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK) +
		sizeof(CDROM_TOC_FULL_TOC_DATA)], point;
    CDROM_TOC_FULL_TOC_DATA *fulltoc = (CDROM_TOC_FULL_TOC_DATA *)buf;
    int i;
    cueify_full_toc_session_private *session;

    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_FULL_TOC;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = TRUE;
    toc_ex.SessionTrack = 1;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;

    /* Can't call IOCTL_CDROM_READ_TOC_EX twice for some reason, so we just
     * have to guess. */
    if (!DeviceIoControl(d->handle,
			 IOCTL_CDROM_READ_TOC_EX,
			 &toc_ex, sizeof(CDROM_READ_TOC_EX),
			 fulltoc, iSize,
			 &dwReturned, NULL)) {
	return CUEIFY_ERR_INTERNAL;
    } else {
	t->first_session_number = fulltoc->FirstCompleteSession;
	t->last_session_number = fulltoc->LastCompleteSession;
	t->first_track_number = t->last_track_number = 0;
	for (i = 0; i < 256; i++) {
	    if (fulltoc->Descriptors[i].Adr == 1) {
		point = fulltoc->Descriptors[i].Point;

		if (point < MAXIMUM_NUMBER_TRACKS) {
		    t->tracks[point].session =
			fulltoc->Descriptors[i].SessionNumber;
		    t->tracks[point].control =
			fulltoc->Descriptors[i].Control;
		    t->tracks[point].adr =
			fulltoc->Descriptors[i].Adr;
		    t->tracks[point].atime.min =
			fulltoc->Descriptors[i].MsfExtra[0];
		    t->tracks[point].atime.sec =
			fulltoc->Descriptors[i].MsfExtra[1];
		    t->tracks[point].atime.frm =
			fulltoc->Descriptors[i].MsfExtra[2];
		    t->tracks[point].offset.min =
			fulltoc->Descriptors[i].Msf[0];
		    t->tracks[point].offset.sec =
			fulltoc->Descriptors[i].Msf[1];
		    t->tracks[point].offset.frm =
			fulltoc->Descriptors[i].Msf[2];
		} else if (point == 0xA0) {
		    session = &(t->sessions[
				    fulltoc->Descriptors[i].SessionNumber]);
		    session->pseudotracks[1].session =
			fulltoc->Descriptors[i].SessionNumber;
		    session->pseudotracks[1].control =
			fulltoc->Descriptors[i].Control;
		    session->pseudotracks[1].adr =
			fulltoc->Descriptors[i].Adr;
		    session->pseudotracks[1].atime.min =
			fulltoc->Descriptors[i].MsfExtra[0];
		    session->pseudotracks[1].atime.sec =
			fulltoc->Descriptors[i].MsfExtra[1];
		    session->pseudotracks[1].atime.frm =
			fulltoc->Descriptors[i].MsfExtra[2];
		    session->first_track_number =
			fulltoc->Descriptors[i].Msf[0];
		    if (t->first_track_number == 0 ||
			session->first_track_number < t->first_track_number) {
			t->first_track_number = session->first_track_number;
		    }
		    session->session_type =
			fulltoc->Descriptors[i].Msf[1];
		} else if (point == 0xA1) {
		    session = &(t->sessions[
				    fulltoc->Descriptors[i].SessionNumber]);
		    session->pseudotracks[2].session =
			fulltoc->Descriptors[i].SessionNumber;
		    session->pseudotracks[2].control =
			fulltoc->Descriptors[i].Control;
		    session->pseudotracks[2].adr =
			fulltoc->Descriptors[i].Adr;
		    session->pseudotracks[2].atime.min =
			fulltoc->Descriptors[i].MsfExtra[0];
		    session->pseudotracks[2].atime.sec =
			fulltoc->Descriptors[i].MsfExtra[1];
		    session->pseudotracks[2].atime.frm =
			fulltoc->Descriptors[i].MsfExtra[2];
		    session->last_track_number =
			fulltoc->Descriptors[i].Msf[0];
		    if (t->last_track_number == 0 ||
			session->last_track_number > t->last_track_number) {
			t->last_track_number = session->last_track_number;
		    }
		} else if (point == 0xA2) {
		    /* Lead-Out */
		    session = &(t->sessions[
				    fulltoc->Descriptors[i].SessionNumber]);
		    session->pseudotracks[0].session =
			fulltoc->Descriptors[i].SessionNumber;
		    session->pseudotracks[0].control =
			fulltoc->Descriptors[i].Control;
		    session->pseudotracks[0].adr =
			fulltoc->Descriptors[i].Adr;
		    session->pseudotracks[0].atime.min =
			fulltoc->Descriptors[i].MsfExtra[0];
		    session->pseudotracks[0].atime.sec =
			fulltoc->Descriptors[i].MsfExtra[1];
		    session->pseudotracks[0].atime.frm =
			fulltoc->Descriptors[i].MsfExtra[2];
		    session->pseudotracks[0].offset.min =
			fulltoc->Descriptors[i].Msf[0];
		    session->pseudotracks[0].offset.sec =
			fulltoc->Descriptors[i].Msf[1];
		    session->pseudotracks[0].offset.frm =
			fulltoc->Descriptors[i].Msf[2];
		    session->leadout = session->pseudotracks[0].offset;
		}
	    }
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_full_toc_unportable */


int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t) {
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    CDROM_TOC_CD_TEXT_DATA dummy;
    CDROM_TOC_CD_TEXT_DATA *cdtext = NULL;

    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_CDTEXT;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = FALSE;
    toc_ex.SessionTrack = 0;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;

    if (DeviceIoControl(d->handle,
			IOCTL_CDROM_READ_TOC_EX,
			&toc_ex, sizeof(CDROM_READ_TOC_EX),
			&dummy, sizeof(dummy),
			&dwReturned, NULL)) {
	cdtext = calloc(1, ((dummy.Length[0] << 8) | dummy.Length[1]) + 2);
	if (cdtext != NULL) {
	    if (!DeviceIoControl(d->handle,
				 IOCTL_CDROM_READ_TOC_EX,
				 &toc_ex, sizeof(CDROM_READ_TOC_EX),
				 cdtext, ((dummy.Length[0] << 8) |
					  dummy.Length[1]) + 2,
				 &dwReturned, NULL)) {
		free(cdtext);
		return CUEIFY_ERR_INTERNAL;
	    }
	}
    }

    /* Can probably get away with this. */
    cueify_cdtext_deserialize((cueify_cdtext *)t, (uint8_t *)cdtext,
			      ((dummy.Length[0] << 8) |
			       dummy.Length[1]) + 2);
    free(cdtext);

    return CUEIFY_OK;
}  /* cueify_device_read_cdtext_unportable */


int cueify_device_read_mcn_unportable(cueify_device_private *d,
				      char *buffer, size_t *size) {
    DWORD dwReturned;
    CDROM_SUB_Q_DATA_FORMAT format;
    SUB_Q_CHANNEL_DATA data;

    format.Track = 0;
    format.Format = IOCTL_CDROM_MEDIA_CATALOG;

    if (!DeviceIoControl(d->handle,
			 IOCTL_CDROM_READ_Q_CHANNEL,
			 &format, sizeof(format),
			 &data, sizeof(data),
			 &dwReturned, NULL)) {
	return CUEIFY_ERR_INTERNAL;
    } else if (!data.MediaCatalog.Mcval) {
	if (*size > 0) {
	    *size = 1;
	    buffer[0] = '\0';
	}
	return CUEIFY_NO_DATA;
    } else {
	*size = min(sizeof(data.MediaCatalog.MediaCatalog) + 1, *size);
	if (*size > 0) {
	    memcpy(buffer, data.MediaCatalog.MediaCatalog, *size - 1);
	    buffer[*size - 1] = '\0';
	}

	return CUEIFY_OK;
    }
}  /* cueify_device_read_mcn_unportable */


int cueify_device_read_isrc_unportable(cueify_device_private *d, uint8_t track,
				       char *buffer, size_t *size) {
    DWORD dwReturned;
    CDROM_SUB_Q_DATA_FORMAT format;
    SUB_Q_CHANNEL_DATA data;

    format.Track = track;
    format.Format = IOCTL_CDROM_TRACK_ISRC;

    if (!DeviceIoControl(d->handle,
			 IOCTL_CDROM_READ_Q_CHANNEL,
			 &format, sizeof(format),
			 &data, sizeof(data),
			 &dwReturned, NULL)) {
	return CUEIFY_ERR_INTERNAL;
    } else if (!data.TrackIsrc.Tcval) {
	if (*size > 0) {
	    *size = 1;
	    buffer[0] = '\0';
	}
	return CUEIFY_NO_DATA;
    } else {
	*size = min(sizeof(data.TrackIsrc.TrackIsrc) + 1, *size);
	if (*size > 0) {
	    memcpy(buffer, data.TrackIsrc.TrackIsrc, *size - 1);
	    buffer[*size - 1] = '\0';
	}

	return CUEIFY_OK;
    }
}  /* cueify_device_read_isrc_unportable */


/** Return the binary representation of a binary-coded decimal. */
#define BCD2BIN(x)  (((x >> 4) & 0xF) * 10 + (x & 0xF))


int cueify_device_read_position_unportable(cueify_device_private *d,
					   uint8_t track, uint32_t lba,
					   cueify_position_t *pos) {
    /* A-la FreeBSD, we now use READ CD, which is at least reproducible?? */
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


#define READ_CD   0xBE  /** MMC op code for READ CD */


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


int cueify_device_read_raw_unportable(cueify_device_private *d, uint32_t lba,
				      cueify_raw_read_private *buffer) {
    /*
     * Could use IOCTL_CDROM_RAW_READ, but the SCSI "READ CD" command
     * is the best way to get the current position.
     */
    SCSI_ADDRESS address;
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER srb;
    struct scsi_read_cd *scsi_cmd;
    DWORD dwReturned;

    memset(&address, 0, sizeof(SCSI_ADDRESS));
    address.Length = sizeof(SCSI_ADDRESS);

    if (!DeviceIoControl(d->handle,
			 IOCTL_SCSI_GET_ADDRESS,
			 &address, sizeof(SCSI_ADDRESS),
			 &address, sizeof(SCSI_ADDRESS),
			 &dwReturned, NULL)) {
	return CUEIFY_ERR_INTERNAL;
    }

    memset(&srb, 0, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    srb.Spt.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    srb.Spt.PathId = address.PathId; /* SCSI Controller ID */
    srb.Spt.TargetId = address.TargetId; /* SCSI Target Device ID */
    srb.Spt.Lun = address.Lun; /* SCSI Logical Unit Device ID */
    srb.Spt.CdbLength = sizeof(struct scsi_read_cd);
    srb.Spt.SenseInfoLength = sizeof(srb.SenseBuf); /* SenseInfo length */
    srb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
    srb.Spt.DataTransferLength = sizeof(cueify_raw_read_private);
    srb.Spt.TimeOutValue = 50000;
    srb.Spt.DataBuffer = buffer;
    srb.Spt.SenseInfoOffset = (UCHAR *)&srb.SenseBuf - (UCHAR *)&srb;

    scsi_cmd = (struct scsi_read_cd *)&srb.Spt.Cdb;
    memset(scsi_cmd, 0, sizeof(*scsi_cmd));

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

    if (!DeviceIoControl(d->handle,
			 IOCTL_SCSI_PASS_THROUGH_DIRECT,
			 &srb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
			 &srb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
			 &dwReturned, NULL)) {
	DWORD error;
	error = GetLastError();
	return CUEIFY_ERR_INTERNAL;
    }

    return CUEIFY_OK;
}  /* cueify_device_read_raw_unportable */
