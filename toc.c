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
#include "toc.h"

int AddressToSectors(UCHAR address[4])
{
    return (((address[1] * 60) + address[2]) * 75) + address[3];
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
	cdtext = malloc(((dummy.Length[0] << 8) | dummy.Length[1]) + 2);
	if (cdtext != NULL) {
	    DeviceIoControl(hDevice,
			    IOCTL_CDROM_READ_TOC_EX,
			    &toc_ex, sizeof(CDROM_READ_TOC_EX),
			    cdtext, ((dummy.Length[0] << 8) | dummy.Length[1]) + 2,
			    &dwReturned, NULL);
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

/** Allocate a character string containing the UTF-8 conversion of the given
 *  wide-character string.
 *
 * @param wchar The wide-character string to convert.
 * @param memsize The length of the wide-character string.
 * @return An allocated character string containing the UTF-8 conversion of
 *         wchar, or NULL if an error occurred.
 */
static unsigned char *wchar_to_utf8(wchar_t *wchar, size_t memsize)
{
    int i, hasSurrogate = 0;
    unsigned long surrogate;
    unsigned char *mem, *ptr;
    
    mem = calloc(memsize * 6 + 1, 1);
    ptr = mem;
    
    for(i = 0; i < memsize; i++)
    {
	if(wchar[i] < 0x80)
	{
	    *ptr++ = wchar[i] & 0x7F;
	}
	else if(wchar[i] < 0x800)
	{
	    *ptr++ = 0xC0 | ((wchar[i] >> 6) & 0x1F);
	    *ptr++ = 0x80 | (wchar[i] & 0x3F);
	}
	else
	{
	    if (wchar[i] >= 0xD800 && wchar[i] <= 0xDBFF)
	    {
		surrogate = (wchar[i] & 0x3FF) << 10;
		hasSurrogate = 1;
	    }
	    else if (hasSurrogate && wchar[i] >= 0xDC00 && wchar[i] <= 0xDFFF)
	    {
		surrogate |= wchar[i] & 0x3FF;
		hasSurrogate = 0;
		if (surrogate < 0x200000)
		{
		    *ptr++ = 0xF0 | ((surrogate >> 18) & 0x07);
		    *ptr++ = 0x80 | ((surrogate >> 12) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 6) & 0x3F);
		    *ptr++ = 0x80 | (surrogate & 0x3F);
		}
		else if(surrogate < 0x4000000)
		{
		    *ptr++ = 0xF8 | ((surrogate >> 24) & 0x03);
		    *ptr++ = 0x80 | ((surrogate >> 18) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 12) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 6) & 0x3F);
		    *ptr++ = 0x80 | (surrogate & 0x3F);
		}
		else if(surrogate < 0x80000000)
		{
		    *ptr++ = 0xFC | ((surrogate >> 30) & 0x01);
		    *ptr++ = 0x80 | ((surrogate >> 24) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 18) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 12) & 0x3F);
		    *ptr++ = 0x80 | ((surrogate >> 6) & 0x3F);
		    *ptr++ = 0x80 | (surrogate & 0x3F);
		}
	    }
	    else
	    {
		if (hasSurrogate)
		{
		    surrogate >>= 10;
		    *ptr++ = 0xE0 | ((surrogate >> 12) & 0x0F);
		    *ptr++ = 0x80 | ((surrogate >> 6) & 0x3F);
		    *ptr++ = 0x80 | (surrogate & 0x3F);
		    hasSurrogate = 0;
		}
		*ptr++ = 0xE0 | ((wchar[i] >> 12) & 0x0F);
		*ptr++ = 0x80 | ((wchar[i] >> 6) & 0x3F);
		*ptr++ = 0x80 | (wchar[i] & 0x3F);
	    }
	}
    }
    
    mem = realloc(mem, ptr - mem + 1);
    
    return mem;
}

struct CDText *ParseCDText(CDROM_TOC_CD_TEXT_DATA *cdtext, int iTracks)
{
    int iDescriptor;
    struct CDText *cdtextData = NULL;
    
    if (((cdtext->Length[0] << 8) | cdtext->Length[1]) > 0) {
	cdtextData = malloc(sizeof(struct CDText));
	if (cdtextData == NULL) {
	    goto ParseCDTextError;
	}
	
	cdtextData->iTracks = iTracks;
	cdtextData->tracks = calloc(iTracks + 1,
				    sizeof(struct CDTextTrack));
	if (cdtextData->tracks == NULL) {
	    goto ParseCDTextError;
	}
	
	for (iDescriptor = 0;
	     iDescriptor * sizeof(CDROM_TOC_CD_TEXT_DATA_BLOCK) < ((cdtext->Length[0] << 8) | cdtext->Length[1]) - (sizeof(cdtext->Reserved1) + sizeof(cdtext->Reserved2));
	     iDescriptor++) {
	    char baseText[25] = "";
	    char *text;
	    int i;
	    
	    if (cdtext->Descriptors[iDescriptor].ExtensionFlag) {
		continue;
	    }
	    
	    /* Parse out the descriptor text into UTF-8. */
	    switch (cdtext->Descriptors[iDescriptor].PackType) {
	    case CDROM_CD_TEXT_PACK_ALBUM_NAME:
	    case CDROM_CD_TEXT_PACK_PERFORMER:
	    case CDROM_CD_TEXT_PACK_SONGWRITER:
	    case CDROM_CD_TEXT_PACK_COMPOSER:
	    case CDROM_CD_TEXT_PACK_ARRANGER:
	    case CDROM_CD_TEXT_PACK_MESSAGES:
	    case CDROM_CD_TEXT_PACK_DISC_ID:
	    case CDROM_CD_TEXT_PACK_GENRE:
	    case CDROM_CD_TEXT_PACK_UPC_EAN:
		if (cdtext->Descriptors[iDescriptor].Unicode) {
		    text = wchar_to_utf8(cdtext->Descriptors[iDescriptor].WText, 6);
		    if (text == NULL) {
			text = strdup(baseText);
		    }
		} else {
		    memcpy(baseText, cdtext->Descriptors[iDescriptor].Text, 12);
		    text = baseText;
		}
		break;
	    default:
		text = cdtext->Descriptors[iDescriptor].Text;
		for (i = 0; i < 12; i++) {
		    if (((text[i] >> 4) & 0xF) < 0xA) {
			baseText[i * 2] = '0' + ((text[i] >> 4) & 0xF);
		    } else {
			baseText[i * 2] = 'A' + (((text[i] >> 4) - 0xA) & 0xF);
		    }
		    if ((text[i] & 0xF) < 0xA) {
			baseText[i * 2 + 1] = '0' + (text[i] & 0xF);
		    } else {
			baseText[i * 2 + 1] = 'A' + ((text[i] & 0xF) - 0xA);
		    }
		}
		text = baseText;
		break;
	    }
	    
	    /* Attach the packet to the correct string. */
	    struct CDTextTrack *track = &(cdtextData->tracks[cdtext->Descriptors[iDescriptor].TrackNumber]);
	    char **datum = NULL;
	    int *datumSize = NULL;
	    
	    switch (cdtext->Descriptors[iDescriptor].PackType) {
	    case CDROM_CD_TEXT_PACK_ALBUM_NAME:
		datum = &(track->title);
		datumSize = &(track->titleSize);
		break;
	    case CDROM_CD_TEXT_PACK_PERFORMER:
		datum = &(track->performer);
		datumSize = &(track->performerSize);
		break;
	    case CDROM_CD_TEXT_PACK_SONGWRITER:
		datum = &(track->songwriter);
		datumSize = &(track->songwriterSize);
		break;
	    case CDROM_CD_TEXT_PACK_COMPOSER:
		datum = &(track->composer);
		datumSize = &(track->composerSize);
		break;
	    case CDROM_CD_TEXT_PACK_ARRANGER:
		datum = &(track->arranger);
		datumSize = &(track->arrangerSize);
		break;
	    case CDROM_CD_TEXT_PACK_MESSAGES:
		datum = &(track->messages);
		datumSize = &(track->messagesSize);
		break;
	    case CDROM_CD_TEXT_PACK_DISC_ID:
		datum = &(track->discId);
		datumSize = &(track->discIdSize);
		break;
	    case CDROM_CD_TEXT_PACK_GENRE:
		datum = &(track->genre);
		datumSize = &(track->genreSize);
		break;
	    case CDROM_CD_TEXT_PACK_TOC_INFO:
		datum = &(track->tocInfo);
		datumSize = &(track->tocInfoSize);
		break;
	    case CDROM_CD_TEXT_PACK_TOC_INFO2:
		datum = &(track->tocInfo2);
		datumSize = &(track->tocInfo2Size);
		break;
	    case CDROM_CD_TEXT_PACK_UPC_EAN:
		datum = &(track->upc_ean);
		datumSize = &(track->upc_eanSize);
		break;
	    case CDROM_CD_TEXT_PACK_SIZE_INFO:
		datum = &(track->sizeInfo);
		datumSize = &(track->sizeInfoSize);
		break;
	    default:
		break;
	    }
	    
	    if (datum != NULL) {
		if (cdtext->Descriptors[iDescriptor].PackType == CDROM_CD_TEXT_PACK_TOC_INFO ||
		    cdtext->Descriptors[iDescriptor].PackType == CDROM_CD_TEXT_PACK_TOC_INFO2 ||
		    cdtext->Descriptors[iDescriptor].PackType == CDROM_CD_TEXT_PACK_SIZE_INFO) {
		    *datum = realloc(*datum, *datumSize + 25);
		    if (*datum == NULL) {
			goto ParseCDTextError;
		    }
		    memcpy(*datum + *datumSize, text, 24);
		    *(*datum + *datumSize + 24) = '\0';
		    *datumSize += 24;
		} else if (cdtext->Descriptors[iDescriptor].Unicode) {
		    *datum = realloc(*datum, *datumSize + 36);
		    if (*datum == NULL) {
			if (cdtext->Descriptors[iDescriptor].Unicode) {
			    free(text);
			}
			goto ParseCDTextError;
		    }
		    memcpy(*datum + *datumSize, text, 36);
		    *datumSize += 36;
		} else {
		    *datum = realloc(*datum, *datumSize + 12);
		    if (*datum == NULL) {
			goto ParseCDTextError;
		    }
		    memcpy(*datum + *datumSize, text, 12);
		    *datumSize += 12;
		}
	    }
	    
	    /* Free the UTF-8 string if need be. */
	    switch (cdtext->Descriptors[iDescriptor].PackType) {
	    case CDROM_CD_TEXT_PACK_ALBUM_NAME:
	    case CDROM_CD_TEXT_PACK_PERFORMER:
	    case CDROM_CD_TEXT_PACK_SONGWRITER:
	    case CDROM_CD_TEXT_PACK_COMPOSER:
	    case CDROM_CD_TEXT_PACK_ARRANGER:
	    case CDROM_CD_TEXT_PACK_MESSAGES:
	    case CDROM_CD_TEXT_PACK_DISC_ID:
	    case CDROM_CD_TEXT_PACK_GENRE:
	    case CDROM_CD_TEXT_PACK_UPC_EAN:
		if (cdtext->Descriptors[iDescriptor].Unicode) {
		    free(text);
		}
		break;
	    default:
		break;
	    }
	}
    }
    
    return cdtextData;
ParseCDTextError:
    FreeCDText(cdtextData);
    return NULL;
}

void FreeCDText(struct CDText *cdtextData)
{
    int iTracks;
    
    if (cdtextData != NULL) {
	if (cdtextData->tracks != NULL) {
	    for (iTracks = 0; iTracks <= cdtextData->iTracks; iTracks++) {
		struct CDTextTrack *track = &(cdtextData->tracks[iTracks]);
		
		if (track->title != NULL) {
		    free(track->title);
		}
		if (track->performer != NULL) {
		    free(track->performer);
		}
		if (track->songwriter != NULL) {
		    free(track->songwriter);
		}
		if (track->composer != NULL) {
		    free(track->composer);
		}
		if (track->arranger != NULL) {
		    free(track->arranger);
		}
		if (track->discId != NULL) {
		    free(track->discId);
		}
		if (track->genre != NULL) {
		    free(track->genre);
		}
		if (track->tocInfo != NULL) {
		    free(track->tocInfo);
		}
		if (track->tocInfo2 != NULL) {
		    free(track->tocInfo2);
		}
		if (track->upc_ean != NULL) {
		    free(track->upc_ean);
		}
		if (track->sizeInfo != NULL) {
		    free(track->sizeInfo);
		}
	    }
	    free(cdtextData->tracks);
	}
	free(cdtextData);
    }
}
