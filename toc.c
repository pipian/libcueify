/* toc.c - Advanced TOC-based CD-ROM functions.
 *
 * Copyright (c) 2010 Ian Jacobi
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
#include "charsets.h"
#include "toc.h"

/** Convert an MSF address into the corresponding LBA address.
 *
 * @param address An MSF address.
 * @return The corresponding LBA address.
 */
static int AddressToSectors(UCHAR address[4])
{
    return (((address[1] * 60) + address[2]) * 75) + address[3];
}

/** Convert an LBA address into the corresponding MSF address.
 *
 * @param sectors An LBA address.
 * @param m A pointer to store the minute value of the MSF address.
 * @param s A pointer to store the second value of the MSF address.
 * @param f A pointer to store the frame value of the MSF address.
 */
static void SectorsToAddress(int sectors, int *m, int *s, int *f)
{
    *m = sectors / (60 * 75);
    *s = sectors / 75 % 60;
    *f = sectors % 75;
}

BOOL ReadLastSession(HANDLE hDevice, CDROM_TOC_SESSION_DATA *session)
{
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    
    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_SESSION;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = TRUE;
    toc_ex.SessionTrack = 0;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;
    
    return DeviceIoControl(hDevice,
			   IOCTL_CDROM_READ_TOC_EX,
			   &toc_ex, sizeof(CDROM_READ_TOC_EX),
			   session, sizeof(CDROM_TOC_SESSION_DATA),
			   &dwReturned, NULL);
}

BOOL ReadTOC(HANDLE hDevice, CDROM_TOC *toc)
{
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    
    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_TOC;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = TRUE;
    toc_ex.SessionTrack = 1;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;
    
    return DeviceIoControl(hDevice,
			   IOCTL_CDROM_READ_TOC_EX,
			   &toc_ex, sizeof(CDROM_READ_TOC_EX),
			   toc, sizeof(CDROM_TOC),
			   &dwReturned, NULL);
}

CDROM_TOC_FULL_TOC_DATA *ReadFullTOC(HANDLE hDevice)
{
    DWORD dwReturned;
    CDROM_READ_TOC_EX toc_ex;
    CDROM_TOC_FULL_TOC_DATA *fulltoc = NULL;
    int iSize = 256 * sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK) + sizeof(CDROM_TOC_FULL_TOC_DATA);
    
    toc_ex.Format = CDROM_READ_TOC_EX_FORMAT_FULL_TOC;
    toc_ex.Reserved1 = 0;
    toc_ex.Msf = TRUE;
    toc_ex.SessionTrack = 1;
    toc_ex.Reserved2 = 0;
    toc_ex.Reserved3 = 0;
    
    /* Can't call IOCTL_CDROM_READ_TOC_EX twice for some reason, so we just
     * have to guess. */
    fulltoc = calloc(1, iSize);
    
    if (fulltoc != NULL) {
	if (!DeviceIoControl(hDevice,
			     IOCTL_CDROM_READ_TOC_EX,
			     &toc_ex, sizeof(CDROM_READ_TOC_EX),
			     fulltoc, iSize,
			     &dwReturned, NULL)) {
	    free(fulltoc);
	    return NULL;
	}
    }
    
    return fulltoc;
}

CDROM_TOC_CD_TEXT_DATA *ReadCDText(HANDLE hDevice)
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
    
    if (DeviceIoControl(hDevice,
			IOCTL_CDROM_READ_TOC_EX,
			&toc_ex, sizeof(CDROM_READ_TOC_EX),
			&dummy, sizeof(dummy),
			&dwReturned, NULL)) {
	cdtext = calloc(1, ((dummy.Length[0] << 8) | dummy.Length[1]) + 2);
	if (cdtext != NULL) {
	    if (!DeviceIoControl(hDevice,
				 IOCTL_CDROM_READ_TOC_EX,
				 &toc_ex, sizeof(CDROM_READ_TOC_EX),
				 cdtext, ((dummy.Length[0] << 8) | dummy.Length[1]) + 2,
				 &dwReturned, NULL)) {
		free(cdtext);
		return NULL;
	    }
	}
    }
    
    return cdtext;
}

BOOL ReadMCN(HANDLE hDevice, SUB_Q_CHANNEL_DATA *data)
{
    DWORD dwReturned;
    CDROM_SUB_Q_DATA_FORMAT format;
    
    format.Track = 0;
    format.Format = IOCTL_CDROM_MEDIA_CATALOG;
    
    return DeviceIoControl(hDevice,
			   IOCTL_CDROM_READ_Q_CHANNEL,
			   &format, sizeof(format),
			   data, sizeof(*data),
			   &dwReturned, NULL);
}

BOOL ReadISRC(HANDLE hDevice, int iTrack, SUB_Q_CHANNEL_DATA *data)
{
    DWORD dwReturned;
    CDROM_SUB_Q_DATA_FORMAT format;
    
    format.Track = iTrack;
    format.Format = IOCTL_CDROM_TRACK_ISRC;
    
    return DeviceIoControl(hDevice,
			   IOCTL_CDROM_READ_Q_CHANNEL,
			   &format, sizeof(format),
			   data, sizeof(*data),
			   &dwReturned, NULL);
}

BOOL ReadCurrentPosition(HANDLE hDevice, int iTrack,
			 int iMinute, int iSecond, int iFrame,
			 SUB_Q_CHANNEL_DATA *data)
{
    DWORD dwReturned;
    CDROM_SEEK_AUDIO_MSF seek;
    CDROM_SUB_Q_DATA_FORMAT format;
    
    /* First, seek to the desired absolute position. */
    seek.M = iMinute;
    seek.S = iSecond;
    seek.F = iFrame;
    if (DeviceIoControl(hDevice,
			IOCTL_CDROM_SEEK_AUDIO_MSF,
			&seek, sizeof(seek),
			NULL, 0,
			&dwReturned, NULL)) {
	/* Next, get the "current" position. */
	format.Track = iTrack;
	format.Format = IOCTL_CDROM_CURRENT_POSITION;
	
	return DeviceIoControl(hDevice,
			       IOCTL_CDROM_READ_Q_CHANNEL,
			       &format, sizeof(format),
			       data, sizeof(*data),
			       &dwReturned, NULL);
    }
    
    return FALSE;
}

/* TODO: Tab characters, Detect pre-gap */
struct CDText *ParseCDText(CDROM_TOC_CD_TEXT_DATA *cdtext)
{
    int iDescriptor, i, j;
    unsigned char *packData[8 * 16];
    unsigned char *pack;
    size_t packSizes[8 * 16];
    size_t packSize;
    struct CDText *cdtextData = NULL;
    CDROM_TOC_CD_TEXT_DATA_BLOCK *descriptor = NULL;
    
    for (i = 0; i < 8 * 16; i++) {
	packData[i] = NULL;
	packSizes[i] = 0;
    }
    
    if (cdtext != NULL &&
	((cdtext->Length[0] << 8) | cdtext->Length[1]) > 0) {
	cdtextData = calloc(1, sizeof(struct CDText));
	if (cdtextData == NULL) {
	    goto ParseCDTextError;
	}
	
	for (iDescriptor = 0;
	     iDescriptor * sizeof(CDROM_TOC_CD_TEXT_DATA_BLOCK) < ((cdtext->Length[0] << 8) | cdtext->Length[1]) - (sizeof(cdtext->Reserved1) + sizeof(cdtext->Reserved2));
	     iDescriptor++) {
	    descriptor = &(cdtext->Descriptors[iDescriptor]);
	    
	    /* Also, ignore extension blocks. */
	    if (descriptor->ExtensionFlag ||
		descriptor->PackType < 0x80 ||
		descriptor->PackType > 0x8F) {
		continue;
	    }
	    
	    pack = 
		packData[descriptor->BlockNumber * 16 +
			 descriptor->PackType - 0x80];
	    packData[descriptor->BlockNumber * 16 +
		     descriptor->PackType - 0x80] = NULL;
	    packSize = packSizes[descriptor->BlockNumber * 16 +
				 descriptor->PackType - 0x80];
	    pack = realloc(pack, packSize + 12);
	    if (pack == NULL) {
		goto ParseCDTextError;
	    }
	    memcpy(pack + packSize, descriptor->Text, 12);
	    packSize += 12;
	    
	    packData[descriptor->BlockNumber * 16 +
		     descriptor->PackType - 0x80] = pack;
	    packSizes[descriptor->BlockNumber * 16 +
		      descriptor->PackType - 0x80] = packSize;
	}
	
	/*
	 * Now that we have collected all of the packs on a per-type
	 * basis, parse the SIZE_INFO packs to get the character code,
	 * track, and language code info.
	 */
	for (i = 0; i < 8; i++) {
	    /* Don't know how to handle any other size! */
	    if (packSizes[i * 16 + 15] != 36) {
		continue;
	    }
	    
	    cdtextData->blocks[i].bValid = 1;
	    cdtextData->blocks[i].charset = packData[i * 16 + 15][0];
	    cdtextData->blocks[i].language = packData[15][i + 28];
	    cdtextData->blocks[i].iTracks = packData[i * 16 + 15][2];
	    cdtextData->blocks[i].bMode2 = (packData[15][3] >> 7) & 1;
	    cdtextData->blocks[i].bProgramCopyright =
		(packData[15][3] >> 6) & 1;
	    cdtextData->blocks[i].bMessageCopyright =
		(packData[15][3] >> 2) & 1;
	    cdtextData->blocks[i].bNameCopyright = (packData[15][3] >> 1) & 1;
	    cdtextData->blocks[i].bTitleCopyright = packData[15][3] & 1;
	    cdtextData->blocks[i].titles =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].titles == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].performers =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].performers == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].songwriters =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].songwriters == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].composers =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].composers == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].arrangers =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].arrangers == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].messages =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].messages == NULL) {
		goto ParseCDTextError;
	    }
	    cdtextData->blocks[i].upc_ean_isrcs =
		calloc(cdtextData->blocks[i].iTracks + 1, sizeof(char *));
	    if (cdtextData->blocks[i].upc_ean_isrcs == NULL) {
		goto ParseCDTextError;
	    }
	}
	
	for (i = 0; i < 8 * 16; i++) {
	    if (packSizes[i] > 0) {
		unsigned char packType = i % 16 + 0x80;
		char **datum;
		char *data = NULL, *dataPtr = NULL;
		
		switch (packType) {
		case CDROM_CD_TEXT_PACK_ALBUM_NAME:
		case CDROM_CD_TEXT_PACK_PERFORMER:
		case CDROM_CD_TEXT_PACK_SONGWRITER:
		case CDROM_CD_TEXT_PACK_COMPOSER:
		case CDROM_CD_TEXT_PACK_ARRANGER:
		case CDROM_CD_TEXT_PACK_MESSAGES:
		case CDROM_CD_TEXT_PACK_UPC_EAN:
		    data = NULL;
		    switch (cdtextData->blocks[i / 16].charset) {
		    case CDROM_CD_TEXT_CHARSET_ASCII:
		    case CDROM_CD_TEXT_CHARSET_ISO8859_1:
			data = ConvertLatin1(packData[i], packSizes[i]);
			if (data == NULL) {
			    goto ParseCDTextError;
			}
			break;
		    case CDROM_CD_TEXT_CHARSET_MSJIS:
			data = ConvertMSJIS(packData[i], packSizes[i] / 2);
			if (data == NULL) {
			    goto ParseCDTextError;
			}
			break;
		    default:
			/* Ignore! */
			break;
		    }
		    
		    /* Split data by track. */
		    switch (packType) {
		    case CDROM_CD_TEXT_PACK_ALBUM_NAME:
			datum = cdtextData->blocks[i / 16].titles;
			break;
		    case CDROM_CD_TEXT_PACK_PERFORMER:
			datum = cdtextData->blocks[i / 16].performers;
			break;
		    case CDROM_CD_TEXT_PACK_SONGWRITER:
			datum = cdtextData->blocks[i / 16].songwriters;
			break;
		    case CDROM_CD_TEXT_PACK_COMPOSER:
			datum = cdtextData->blocks[i / 16].composers;
			break;
		    case CDROM_CD_TEXT_PACK_ARRANGER:
			datum = cdtextData->blocks[i / 16].arrangers;
			break;
		    case CDROM_CD_TEXT_PACK_MESSAGES:
			datum = cdtextData->blocks[i / 16].messages;
			break;
		    case CDROM_CD_TEXT_PACK_UPC_EAN:
			datum = cdtextData->blocks[i / 16].upc_ean_isrcs;
			break;
		    default:
			break;
		    }
		    
		    if (data != NULL) {
			dataPtr = data;
			for (j = 0;
			     j < cdtextData->blocks[i / 16].iTracks + 1;
			     j++) {
			    datum[j] = strdup(dataPtr);
			    if (datum[j] == NULL) {
				goto ParseCDTextError;
			    }
			    dataPtr += strlen(dataPtr) + 1;
			}
			free(data);
		    }
		    break;
		case CDROM_CD_TEXT_PACK_DISC_ID:
		    cdtextData->blocks[i / 16].discID =
			ConvertLatin1(packData[i], packSizes[i]);
		    break;
		case CDROM_CD_TEXT_PACK_GENRE:
		    /* Genre includes a genre code in addition to text. */
		    cdtextData->blocks[i / 16].genreCode =
			((unsigned char)packData[i][0] << 8) |
			((unsigned char)packData[i][1]);
		    cdtextData->blocks[i / 16].genreName =
			ConvertLatin1(packData[i] + 2, packSizes[i] - 2);
		    break;
		case CDROM_CD_TEXT_PACK_TOC_INFO:
		    /* This basically encodes another (short) TOC. */
		    cdtextData->tocInfo.firstTrack = packData[i][0];
		    cdtextData->tocInfo.lastTrack = packData[i][1];
		    cdtextData->tocInfo.trackPointers[0].M = packData[i][3];
		    cdtextData->tocInfo.trackPointers[0].S = packData[i][4];
		    cdtextData->tocInfo.trackPointers[0].F = packData[i][5];
		    for (j = 0; j < cdtextData->tocInfo.lastTrack; j++) {
			cdtextData->tocInfo.trackPointers[j + 1].M =
			    packData[i][12 + j * 3];
			cdtextData->tocInfo.trackPointers[j + 1].S =
			    packData[i][12 + j * 3 + 1];
			cdtextData->tocInfo.trackPointers[j + 1].F =
			    packData[i][12 + j * 3 + 2];
		    }
		    break;
		case CDROM_CD_TEXT_PACK_TOC_INFO2:
		    cdtextData->tocInfo2.iIntervals =
			packSizes[i] / 12;
		    cdtextData->tocInfo2.intervals =
			realloc(cdtextData->tocInfo2.intervals,
				cdtextData->tocInfo2.iIntervals *
				sizeof(struct CDTextTOCInterval));
		    if (cdtextData->tocInfo2.intervals != NULL) {
			memset(&(cdtextData->tocInfo2.intervals), 0,
			       cdtextData->tocInfo2.iIntervals *
			       sizeof(struct CDTextTOCInterval));
			for (j = 0; j < cdtextData->tocInfo2.iIntervals; j++) {
			    cdtextData->tocInfo2.intervals[j].priorityNumber =
				packData[i][j * 12];
			    cdtextData->tocInfo2.intervals[i].numIntervals =
				packData[i][j * 12 + 1];
			    cdtextData->tocInfo2.intervals[i].start.M =
				packData[i][j * 12 + 6];
			    cdtextData->tocInfo2.intervals[i].start.S =
				packData[i][j * 12 + 7];
			    cdtextData->tocInfo2.intervals[i].start.F =
				packData[i][j * 12 + 8];
			    cdtextData->tocInfo2.intervals[i].end.M =
				packData[i][j * 12 + 9];
			    cdtextData->tocInfo2.intervals[i].end.S =
				packData[i][j * 12 + 10];
			    cdtextData->tocInfo2.intervals[i].end.F =
				packData[i][j * 12 + 11];
			}
		    }
		    break;
		case CDROM_CD_TEXT_PACK_SIZE_INFO:
		default:
		    /* Ignore! */
		    break;
		}
	    }
	}
    }
    
    for (i = 0; i < 8 * 16; i++) {
	free(packData[i]);
    }
    return cdtextData;
ParseCDTextError:
    for (i = 0; i < 8 * 16; i++) {
	free(packData[i]);
    }
    FreeCDText(cdtextData);
    return NULL;
}

void FreeCDText(struct CDText *cdtextData)
{
    int i, j;
    
    if (cdtextData != NULL) {
	for (i = 0; i < 8; i++) {
	    for (j = 0; j < cdtextData->blocks[i].iTracks; j++) {
		if (cdtextData->blocks[i].titles != NULL) {
		    free(cdtextData->blocks[i].titles[j]);
		}
		if (cdtextData->blocks[i].performers != NULL) {
		    free(cdtextData->blocks[i].performers[j]);
		}
		if (cdtextData->blocks[i].songwriters != NULL) {
		    free(cdtextData->blocks[i].songwriters[j]);
		}
		if (cdtextData->blocks[i].composers != NULL) {
		    free(cdtextData->blocks[i].composers[j]);
		}
		if (cdtextData->blocks[i].arrangers != NULL) {
		    free(cdtextData->blocks[i].arrangers[j]);
		}
		if (cdtextData->blocks[i].messages != NULL) {
		    free(cdtextData->blocks[i].messages[j]);
		}
		if (cdtextData->blocks[i].upc_ean_isrcs != NULL) {
		    free(cdtextData->blocks[i].upc_ean_isrcs[j]);
		}
	    }
	    free(cdtextData->blocks[i].titles);
	    free(cdtextData->blocks[i].performers);
	    free(cdtextData->blocks[i].songwriters);
	    free(cdtextData->blocks[i].composers);
	    free(cdtextData->blocks[i].arrangers);
	    free(cdtextData->blocks[i].messages);
	    free(cdtextData->blocks[i].upc_ean_isrcs);
	    free(cdtextData->blocks[i].discID);
	    free(cdtextData->blocks[i].genreName);
	}
	free(cdtextData->tocInfo2.intervals);
	free(cdtextData);
    }
}

BOOL DetectTrackIndices(HANDLE hDevice, CDROM_TOC *toc, int iTrack,
			struct TrackIndices *indices)
{
    if (iTrack <= toc->LastTrack) {
	/* First, see if there appears to be more than one index. */
	SUB_Q_CHANNEL_DATA data;
	int iMinute, iSecond, iFrame;
	int iLBA, iFirstLBA, iLeftLBA, iRightLBA, iLastLBA;
	int iIndex;
	
	iFirstLBA = iLeftLBA =
	    AddressToSectors(toc->TrackData[iTrack - 1].Address);
	iLastLBA = iRightLBA =
	    AddressToSectors(toc->TrackData[iTrack].Address);
	
	/* Get the index of the penultimate second of the track. */
	iLBA = iLastLBA - 1;
	if (iLBA < iLeftLBA) {
	    iLBA = (iLeftLBA + iLastLBA) / 2;
	}
	
	SectorsToAddress(iLBA, &iMinute, &iSecond, &iFrame);
	
	if (!ReadCurrentPosition(hDevice, iTrack, iMinute, iSecond, iFrame,
				 &data)) {
	    return FALSE;
	}
	
	if (data.CurrentPosition.TrackNumber == iTrack &&
	    data.CurrentPosition.IndexNumber != 1) {
	    indices->iIndices = data.CurrentPosition.IndexNumber;
	    indices->bHasPregap = FALSE;
	    indices->indices = calloc(indices->iIndices,
				      sizeof(struct TrackIndex));
	    if (indices->indices == NULL) {
		return FALSE;
	    }
	    indices->indices[0].M = toc->TrackData[iTrack - 1].Address[1];
	    indices->indices[0].S = toc->TrackData[iTrack - 1].Address[2];
	    indices->indices[0].F = toc->TrackData[iTrack - 1].Address[3];
	} else if (data.CurrentPosition.TrackNumber == iTrack + 1) {
	    indices->iIndices = 2;
	    indices->bHasPregap = TRUE;
	    indices->indices = calloc(indices->iIndices,
				      sizeof(struct TrackIndex));
	    if (indices->indices == NULL) {
		return FALSE;
	    }
	    indices->indices[0].M = toc->TrackData[iTrack - 1].Address[1];
	    indices->indices[0].S = toc->TrackData[iTrack - 1].Address[2];
	    indices->indices[0].F = toc->TrackData[iTrack - 1].Address[3];
	    
	    /* Detect the pre-gap. */
	    while (iLeftLBA != iRightLBA) {
		iLBA = (iLeftLBA + iRightLBA) / 2;
		SectorsToAddress(iLBA, &iMinute, &iSecond, &iFrame);
		
		if (!ReadCurrentPosition(hDevice, iTrack,
					 iMinute, iSecond, iFrame,
					 &data)) {
		    free(indices->indices);
		    indices->indices = NULL;
		    return FALSE;
		}
		
		if (data.CurrentPosition.TrackNumber == iTrack) {
		    /* Choose the right half. */
		    iLeftLBA = iLBA + 1;
		} else {
		    /* Choose the left half. */
		    iRightLBA = iLBA;
		}
	    }
	    
	    /* Found the start address. */
	    SectorsToAddress(iLeftLBA, &iMinute, &iSecond, &iFrame);
	    indices->indices[1].M = iMinute;
	    indices->indices[1].S = iSecond;
	    indices->indices[1].F = iFrame;
	    
	    /* Reset the right and left sides. */
	    iLastLBA = iRightLBA = iLBA;
	    iLeftLBA = iFirstLBA;
	    
	    /* And calculate the TRUE index count. */
	    iLBA = iLastLBA - 1;
	    if (iLBA < iLeftLBA) {
		iLBA = (iLeftLBA + iLastLBA) / 2;
	    }
	    
	    SectorsToAddress(iLBA, &iMinute, &iSecond, &iFrame);
	    
	    if (!ReadCurrentPosition(hDevice, iTrack, iMinute, iSecond, iFrame,
				     &data)) {
		return FALSE;
	    }
	    
	    if (data.CurrentPosition.TrackNumber == iTrack &&
		data.CurrentPosition.IndexNumber != 1) {
		indices->iIndices = data.CurrentPosition.IndexNumber + 1;
		indices->indices = realloc(indices->indices,
					   indices->iIndices * sizeof(struct TrackIndex));
		if (indices->indices == NULL) {
		    return FALSE;
		}
		
		/* TODO: Test me. */
		/* Copy the pre-gap indices over. */
		memcpy(&(indices->indices[indices->iIndices]),
		       &(indices->indices[1]),
		       sizeof(struct TrackIndex));
	    } else {
		/* That's it. */
		return TRUE;
	    }
	} else {
	    indices->iIndices = 1;
	    indices->bHasPregap = FALSE;
	    indices->indices = calloc(1, sizeof(struct TrackIndex));
	    if (indices->indices == NULL) {
		return FALSE;
	    }
	    indices->indices[0].M = toc->TrackData[iTrack - 1].Address[1];
	    indices->indices[0].S = toc->TrackData[iTrack - 1].Address[2];
	    indices->indices[0].F = toc->TrackData[iTrack - 1].Address[3];
	    return TRUE;
	}
	
	for (iIndex = 1; iIndex < indices->iIndices; iIndex++) {
	    /* Detect the given index by binary search. */
	    while (iLeftLBA != iRightLBA) {
		iLBA = (iLeftLBA + iRightLBA) / 2;
		SectorsToAddress(iLBA, &iMinute, &iSecond, &iFrame);
		
		if (!ReadCurrentPosition(hDevice, iTrack,
					 iMinute, iSecond, iFrame,
					 &data)) {
		    free(indices->indices);
		    indices->indices = NULL;
		    return FALSE;
		}
		
		if (data.CurrentPosition.IndexNumber >= iIndex + 1) {
		    /* Choose the left half. */
		    iRightLBA = iLBA;
		} else {
		    /* Choose the right half. */
		    iLeftLBA = iLBA + 1;
		}
	    }
	    
	    /* Found the start address. */
	    SectorsToAddress(iLeftLBA, &iMinute, &iSecond, &iFrame);
	    indices->indices[iIndex].M = iMinute;
	    indices->indices[iIndex].S = iSecond;
	    indices->indices[iIndex].F = iFrame;
	    
	    /* Reset the right side. */
	    iRightLBA = iLastLBA;
	}
	
	return TRUE;
    } else {
	return FALSE;
    }
}
