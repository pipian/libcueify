/* win32.c - win32-specific (Windows XP+) CD-ROM API glue
 *
 * Copyright (c) 2010, 2011 Ian Jacobi
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
#include <tchar.h>

LPCTSTR szVolumeFormat[9] = "\\\\.\\%lc:";
LPCTSTR szRootFormat[6] = "%lc:\\";

int cueify_device_open_unportable(cueify_device_private *d,
				  const char *device) {
    HANDLE hVolume;
    WCHAR cDriveLetter;
    UINT uDriveType;
    TCHAR szVolumeName[7];
    TCHAR szRootName[4];
    DWORD dwAccessFlags;

    /* Precondition: device[0] != '\0' */

    /* Is it a drive letter? (Regular Expression: c(?::\?\)?) */
    if ( device[1] == '\0' ||
	(device[1] == ':' && device[2] == '\0')) {
	/* Just a single letter. */
	cDriveLetter = device[0];
    } else {
	/* We currently don't handle drives without a letter. */
	return CUEIFY_BADARG;
    }
    wsprintf(szRootName, szRootFormat, cDriveLetter);

    uDriveType = GetDriveType(szRootName);
    switch (uDriveType) {
    case DRIVE_REMOVABLE:
	dwAccessFlags = GENERIC_READ | GENERIC_WRITE;
	break;
    case DRIVE_CDROM:
	dwAccessFlags = GENERIC_READ;
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

const char *driveLetters[26] = {
    "a:\\", "b:\\", "c:\\", "d:\\", "e:\\", "f:\\", "g:\\", "h:\\",
    "i:\\", "j:\\", "k:\\", "l:\\", "m:\\", "n:\\", "o:\\", "p:\\",
    "q:\\", "r:\\", "s:\\", "t:\\", "u:\\", "v:\\", "w:\\", "x:\\",
    "y:\\", "z:\\"
};

const char *cueify_device_get_default_device_unportable() {
    int i;
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
    BOOL succeded;
    int i;
    
    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_TOC;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = FALSE;
    toc_ex.SessionTrack = 1;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;
    
    succeded = DeviceIoControl(d->handle,
			       IOCTL_CDROM_READ_TOC_EX,
			       &toc_ex, sizeof(CDROM_READ_TOC_EX),
			       &toc, sizeof(CDROM_TOC),
			       &dwReturned, NULL);
    if (!succeeded) {
	return CUEIFY_ERR_INTERNAL;
    } else {
	t->first_track_number = toc->FirstTrack;
	t->last_track_number = toc->LastTrack;
	for (i = 0; i < MAXIMUM_NUMBER_TRACKS; i++) {
	    if (toc->TrackData[i].TrackNumber == 0xAA) {
		/* Lead-out Track */
		t->tracks[0].control = toc->TrackData[i].Control;
		t->tracks[0].adr = toc->TrackData[i].Adr;
		t->tracks[0].lba =
		    ((toc->TrackData[i].Address[0] << 24) |
		     (toc->TrackData[i].Address[1] << 16) |
		     (toc->TrackData[i].Address[2] << 8) |
		     toc->TrackData[i].Address[3]);
	    } else if (toc->TrackData[i].TrackNumber != 0) {
		t->tracks[toc->TrackData[i].TrackNumber].control =
		    toc->TrackData[i].Control;
		t->tracks[toc->TrackData[i].TrackNumber].adr =
		    toc->TrackData[i].Adr;
		t->tracks[toc->TrackData[i].TrackNumber].lba =
		    ((toc->TrackData[i].Address[0] << 24) |
		     (toc->TrackData[i].Address[1] << 16) |
		     (toc->TrackData[i].Address[2] << 8) |
		     toc->TrackData[i].Address[3]);
	    }
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_toc_unportable */


int cueify_device_read_sessions_unportable(cueify_device_private *d,
					   cueify_sessions_private *s) {
{
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    CDROM_TOC_SESSION_DATA session;
    BOOL succeded;

    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_SESSION;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = FALSE;
    toc_ex.SessionTrack = 0;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;

    succeeded = DeviceIoControl(d->device,
				IOCTL_CDROM_READ_TOC_EX,
				&toc_ex, sizeof(CDROM_READ_TOC_EX),
				&session, sizeof(CDROM_TOC_SESSION_DATA),
				&dwReturned, NULL);
    if (!succeeded) {
	return CUEIFY_ERR_INTERNAL;
    } else {
	s->first_session_number = session->FirstCompleteSession;
	s->last_session_number = session->LastCompleteSession;
	s->track_control = session->TrackData[0].Control;
	s->track_adr = session->TrackData[0].Adr;
	s->track_number = session->TrackData[0].TrackNumber;
	s->track_lba =
	  ((session->TrackData[0].Address[0] << 24) |
	   (session->TrackData[0].Address[1] << 16) |
	   (session->TrackData[0].Address[2] << 8) |
	   session->TrackData[0].Address[3]);
    }

    return CUEIFY_OK;
}  /* cueify_device_read_sessions_unportable */


int cueify_device_read_full_toc_unportable(cueify_device_private *d,
					   cueify_full_toc_private *t) {
{
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    int iSize = 256 * sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK) +
	sizeof(CDROM_TOC_FULL_TOC_DATA);
    uint8_t buf[iSize], point;
    CDROM_TOC_FULL_TOC_DATA *fulltoc = buf;
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
		    if (session->first_track_number == 0 ||
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
		    if (session->last_track_number == 0 ||
			session->last_track_number > t->last_track_number) {
			t->last_track_number = session->last_track_number;
		    }
		    session->last_track_number =
			fulltoc->Descriptors[i].Msf[0];
		} else if (point == 0xA2) {
		    /* Lead-Out */
		    session = &(t->sessions[
				    fulltoc->Descriptors[i].SessionNumber]);
		    session->pseudotracks[3].session =
			fulltoc->Descriptors[i].SessionNumber;
		    session->pseudotracks[3].control =
			fulltoc->Descriptors[i].Control;
		    session->pseudotracks[3].adr =
			fulltoc->Descriptors[i].Adr;
		    session->pseudotracks[3].atime.min =
			fulltoc->Descriptors[i].MsfExtra[0];
		    session->pseudotracks[3].atime.sec =
			fulltoc->Descriptors[i].MsfExtra[1];
		    session->pseudotracks[3].atime.frm =
			fulltoc->Descriptors[i].MsfExtra[2];
		    session->pseudotracks[3].offset.min =
			fulltoc->Descriptors[i].Msf[0];
		    session->pseudotracks[3].offset.sec =
			fulltoc->Descriptors[i].Msf[1];
		    session->pseudotracks[3].offset.frm =
			fulltoc->Descriptors[i].Msf[2];
		    session->leadout = session->pseudotracks[3].offset;
		}
	    }
	}
    }

    return CUEIFY_OK;
}  /* cueify_device_read_full_toc_unportable */


int cueify_device_read_cdtext_unportable(cueify_device_private *d,
					 cueify_cdtext_private *t) {
{
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
    cueify_cdtext_deserialize(t, cdtext,
			      ((dummy.Length[0] << 8) |
			       dummy.Length[1]) + 2);
    free(cdtext);

    return CUEIFY_OK;
}  /* cueify_device_read_cdtext_unportable */
