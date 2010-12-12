/* toc.h - Header for advanced TOC-based CD-ROM functions.
 *
 * Copyright (c) 2010 Ian Jacobi, except typedefs and defines, which are
 *     in the public domain courtesy of Caspar S. Hornstrup,
 *     <chorns@users.sourceforge.net>
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

#ifndef _7604DRIVERS_TOC_H
#define _7604DRIVERS_TOC_H

#include <windows.h>

#define CTL_CODE(DeviceType, Function, Method, Access) \
    (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define FILE_DEVICE_CD_ROM                0x00000002
#define FILE_READ_ACCESS                  0x00000001
#define METHOD_BUFFERED                   0

#define IOCTL_CDROM_BASE                  FILE_DEVICE_CD_ROM

#define IOCTL_CDROM_READ_TOC_EX \
    CTL_CODE(IOCTL_CDROM_BASE, 0x0015, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_READ_Q_CHANNEL \
    CTL_CODE(IOCTL_CDROM_BASE, 0x000B, METHOD_BUFFERED, FILE_READ_ACCESS)

#define MAXIMUM_NUMBER_TRACKS             100

typedef struct _TRACK_DATA {
    UCHAR  Reserved;
    UCHAR  Control : 4;
    UCHAR  Adr : 4;
    UCHAR  TrackNumber;
    UCHAR  Reserved1;
    UCHAR  Address[4];
} TRACK_DATA, *PTRACK_DATA;

/* CDROM_READ_TOC_EX.Format constants */
#define CDROM_READ_TOC_EX_FORMAT_TOC      0x00
#define CDROM_READ_TOC_EX_FORMAT_SESSION  0x01
#define CDROM_READ_TOC_EX_FORMAT_FULL_TOC 0x02
#define CDROM_READ_TOC_EX_FORMAT_PMA      0x03
#define CDROM_READ_TOC_EX_FORMAT_ATIP     0x04
#define CDROM_READ_TOC_EX_FORMAT_CDTEXT   0x05

typedef struct _CDROM_READ_TOC_EX {
    UCHAR  Format : 4;
    UCHAR  Reserved1 : 3;
    UCHAR  Msf : 1;
    UCHAR  SessionTrack;
    UCHAR  Reserved2;
    UCHAR  Reserved3;
} CDROM_READ_TOC_EX, *PCDROM_READ_TOC_EX;

/* CDROM_SUB_Q_DATA_FORMAT.Format constants */
#define IOCTL_CDROM_SUB_Q_CHANNEL         0x00
#define IOCTL_CDROM_CURRENT_POSITION      0x01
#define IOCTL_CDROM_MEDIA_CATALOG         0x02
#define IOCTL_CDROM_TRACK_ISRC            0x03

typedef struct _CDROM_SUB_Q_DATA_FORMAT {
    UCHAR Format;
    UCHAR Track;
} CDROM_SUB_Q_DATA_FORMAT, *PCDROM_SUB_Q_DATA_FORMAT;

typedef struct _CDROM_TOC {
    UCHAR  Length[2];
    UCHAR  FirstTrack;
    UCHAR  LastTrack;
    TRACK_DATA  TrackData[MAXIMUM_NUMBER_TRACKS];
} CDROM_TOC, *PCDROM_TOC;

typedef struct _CDROM_TOC_SESSION_DATA {
    UCHAR Length[2];
    UCHAR FirstCompleteSession;
    UCHAR LastCompleteSession;
    TRACK_DATA TrackData[1];
} CDROM_TOC_SESSION_DATA, *PCDROM_TOC_SESSION_DATA;

/* CDROM_TOC_CD_TEXT_DATA_BLOCK.PackType constants */
#define CDROM_CD_TEXT_PACK_ALBUM_NAME 0x80
#define CDROM_CD_TEXT_PACK_PERFORMER  0x81
#define CDROM_CD_TEXT_PACK_SONGWRITER 0x82
#define CDROM_CD_TEXT_PACK_COMPOSER   0x83
#define CDROM_CD_TEXT_PACK_ARRANGER   0x84
#define CDROM_CD_TEXT_PACK_MESSAGES   0x85
#define CDROM_CD_TEXT_PACK_DISC_ID    0x86
#define CDROM_CD_TEXT_PACK_GENRE      0x87
#define CDROM_CD_TEXT_PACK_TOC_INFO   0x88
#define CDROM_CD_TEXT_PACK_TOC_INFO2  0x89
#define CDROM_CD_TEXT_PACK_UPC_EAN    0x8e
#define CDROM_CD_TEXT_PACK_SIZE_INFO  0x8f

typedef struct _CDROM_TOC_CD_TEXT_DATA_BLOCK {
    UCHAR  PackType;
    UCHAR  TrackNumber : 7;
    UCHAR  ExtensionFlag : 1;
    UCHAR  SequenceNumber;
    UCHAR  CharacterPosition : 4;
    UCHAR  BlockNumber : 3;
    UCHAR  Unicode : 1;
    _ANONYMOUS_UNION union {
	UCHAR  Text[12];
	WCHAR  WText[6];
    } DUMMYUNIONNAME;
    UCHAR  CRC[2];
} CDROM_TOC_CD_TEXT_DATA_BLOCK, *PCDROM_TOC_CD_TEXT_DATA_BLOCK;

typedef struct _CDROM_TOC_CD_TEXT_DATA {
    UCHAR  Length[2];
    UCHAR  Reserved1;
    UCHAR  Reserved2;
    CDROM_TOC_CD_TEXT_DATA_BLOCK  Descriptors[0];
} CDROM_TOC_CD_TEXT_DATA, *PCDROM_TOC_CD_TEXT_DATA;

/* SUB_Q_HEADER.AudioStatus constants */
#define AUDIO_STATUS_NOT_SUPPORTED  0x00
#define AUDIO_STATUS_IN_PROGRESS    0x11
#define AUDIO_STATUS_PAUSED         0x12
#define AUDIO_STATUS_PLAY_COMPLETE  0x13
#define AUDIO_STATUS_PLAY_ERROR     0x14
#define AUDIO_STATUS_NO_STATUS      0x15

typedef struct _SUB_Q_HEADER {
    UCHAR  Reserved;
    UCHAR  AudioStatus;
    UCHAR  DataLength[2];
} SUB_Q_HEADER, *PSUB_Q_HEADER;

typedef struct _SUB_Q_MEDIA_CATALOG_NUMBER {
    SUB_Q_HEADER  Header;
    UCHAR  FormatCode;
    UCHAR  Reserved[3];
    UCHAR  Reserved1 : 7;
    UCHAR  Mcval :1;
    UCHAR  MediaCatalog[15];
} SUB_Q_MEDIA_CATALOG_NUMBER, *PSUB_Q_MEDIA_CATALOG_NUMBER;

typedef struct _SUB_Q_TRACK_ISRC {
    SUB_Q_HEADER  Header;
    UCHAR  FormatCode;
    UCHAR  Reserved0;
    UCHAR  Track;
    UCHAR  Reserved1;
    UCHAR  Reserved2 : 7;
    UCHAR  Tcval : 1;
    UCHAR  TrackIsrc[15];
} SUB_Q_TRACK_ISRC, *PSUB_Q_TRACK_ISRC;

typedef struct _SUB_Q_CURRENT_POSITION {
    SUB_Q_HEADER  Header;
    UCHAR  FormatCode;
    UCHAR  Control : 4;
    UCHAR  ADR : 4;
    UCHAR  TrackNumber;
    UCHAR  IndexNumber;
    UCHAR  AbsoluteAddress[4];
    UCHAR  TrackRelativeAddress[4];
} SUB_Q_CURRENT_POSITION, *PSUB_Q_CURRENT_POSITION;
 
typedef union _SUB_Q_CHANNEL_DATA {
    SUB_Q_CURRENT_POSITION  CurrentPosition;
    SUB_Q_MEDIA_CATALOG_NUMBER  MediaCatalog;
    SUB_Q_TRACK_ISRC  TrackIsrc;
} SUB_Q_CHANNEL_DATA, *PSUB_Q_CHANNEL_DATA;

/** Convert an MSF address into the corresponding LBA address.
 *
 * @param address An MSF address.
 * @return The corresponding LBA address.
 */
int AddressToSectors(UCHAR address[4]);

/** Read the contents of the session data into a
 *  CDROM_TOC_SESSION_DATA variable provided.
 *
 * @param hDevice A handle to the drive to read the session data from.
 * @param session A pointer to the CDROM_TOC_SESSION_DATA to populate.
 * @return TRUE if the command succeeded.
 */
BOOL ReadLastSession(HANDLE hDevice, CDROM_TOC_SESSION_DATA *session);

/** Read the contents of the CD-ROM TOC into a CDROM_TOC variable
 *  provided.
 *
 * @param hDevice A handle to the drive to read the TOC data from.
 * @param toc A pointer to the CDROM_TOC to populate.
 * @return TRUE if the command succeeded.
 */
BOOL ReadTOC(HANDLE hDevice, CDROM_TOC *toc);

/** Read the contents of the CD-Text into a CDROM_TOC_CD_TEXT_DATA
 *  variable provided.
 *
 * @param hDevice A handle to the drive to read the CD-Text data from.
 * @param cdtext A pointer to the CDROM_TOC_CD_TEXT_DATA to populate.
 * @param size The size of the CDROM_TOC_CD_TEXT_DATA pointer.
 * @return TRUE if the command succeeded.
 */
BOOL ReadCDText(HANDLE hDevice, CDROM_TOC_CD_TEXT_DATA *cdtext, size_t size);

/** Read the CD-ROM's MCN into a SUB_Q_CHANNEL_DATA variable provided.
 *
 * @param hDevice A handle to the drive to read the MCN from.
 * @param data A pointer to the SUB_Q_CHANNEL_DATA to populate.
 * @return TRUE if the command succeeded.
 */
BOOL ReadMCN(HANDLE hDevice, SUB_Q_CHANNEL_DATA *data);

/** Read an ISRC into a SUB_Q_CHANNEL_DATA variable provided.
 *
 * @param hDevice A handle to the drive to read the ISRC from.
 * @param iTrack The track to read the ISRC from.
 * @param data A pointer to the SUB_Q_CHANNEL_DATA to populate.
 * @return TRUE if the command succeeded.
 */
BOOL ReadISRC(HANDLE hDevice, int iTrack, SUB_Q_CHANNEL_DATA *data);

#endif
