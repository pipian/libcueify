/* win32.h - Header for win32-specific (Windows XP+) CD-ROM API glue
 *
 * Copyright (c) 2010, 2011 Ian Jacobi, except typedefs and defines,
 *     which are in the public domain courtesy of Caspar S. Hornstrup,
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

#ifndef _LIBCUEIFY_WIN32_H
#define _LIBCUEIFY_WIN32_H

#include <windows.h>

#define CTL_CODE(DeviceType, Function, Method, Access) \
    (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define FILE_DEVICE_CD_ROM                0x00000002
#define FILE_READ_ACCESS                  0x00000001
#define METHOD_BUFFERED                   0
#define METHOD_OUT_DIRECT                 2

#define IOCTL_CDROM_BASE                  FILE_DEVICE_CD_ROM

#define IOCTL_CDROM_READ_Q_CHANNEL \
    CTL_CODE(IOCTL_CDROM_BASE, 0x000B, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_READ_TOC_EX \
    CTL_CODE(IOCTL_CDROM_BASE, 0x0015, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_SEEK_AUDIO_MSF \
    CTL_CODE(IOCTL_CDROM_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_RAW_READ \
    CTL_CODE(IOCTL_CDROM_BASE, 0x000F, METHOD_OUT_DIRECT, FILE_READ_ACCESS)

#define MAXIMUM_NUMBER_TRACKS             100

#ifdef __GNUC__
__extension__
#endif
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

#ifdef __GNUC__
__extension__
#endif
typedef struct _CDROM_READ_TOC_EX {
    UCHAR  Format : 4;
    UCHAR  Reserved1 : 3;
    UCHAR  Msf : 1;
    UCHAR  SessionTrack;
    UCHAR  Reserved2;
    UCHAR  Reserved3;
} CDROM_READ_TOC_EX, *PCDROM_READ_TOC_EX;

typedef struct _CDROM_SEEK_AUDIO_MSF {
  UCHAR  M;
  UCHAR  S;
  UCHAR  F;
} CDROM_SEEK_AUDIO_MSF, *PCDROM_SEEK_AUDIO_MSF;

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

#ifdef __GNUC__
__extension__
#endif
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
    CDROM_TOC_CD_TEXT_DATA_BLOCK  Descriptors[];
} CDROM_TOC_CD_TEXT_DATA, *PCDROM_TOC_CD_TEXT_DATA;

#ifdef __GNUC__
__extension__
#endif
typedef struct _CDROM_TOC_FULL_TOC_DATA_BLOCK {
  UCHAR  SessionNumber;
  UCHAR  Control : 4;
  UCHAR  Adr : 4;
  UCHAR  Reserved1;
  UCHAR  Point;
  UCHAR  MsfExtra[3];
  UCHAR  Zero;
  UCHAR  Msf[3];
} CDROM_TOC_FULL_TOC_DATA_BLOCK, *PCDROM_TOC_FULL_TOC_DATA_BLOCK;

typedef struct _CDROM_TOC_FULL_TOC_DATA {
  UCHAR  Length[2];
  UCHAR  FirstCompleteSession;
  UCHAR  LastCompleteSession;
  CDROM_TOC_FULL_TOC_DATA_BLOCK  Descriptors[];
} CDROM_TOC_FULL_TOC_DATA, *PCDROM_TOC_FULL_TOC_DATA;

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

#ifdef __GNUC__
__extension__
#endif
typedef struct _SUB_Q_MEDIA_CATALOG_NUMBER {
    SUB_Q_HEADER  Header;
    UCHAR  FormatCode;
    UCHAR  Reserved[3];
    UCHAR  Reserved1 : 7;
    UCHAR  Mcval :1;
    UCHAR  MediaCatalog[15];
} SUB_Q_MEDIA_CATALOG_NUMBER, *PSUB_Q_MEDIA_CATALOG_NUMBER;

#ifdef __GNUC__
__extension__
#endif
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

#ifdef __GNUC__
__extension__
#endif
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

typedef enum _TRACK_MODE_TYPE {
    YellowMode2,
    XAForm2,
    CDDA
} TRACK_MODE_TYPE, *PTRACK_MODE_TYPE;

typedef struct __RAW_READ_INFO {
    LARGE_INTEGER DiskOffset;
    ULONG SectorCount;
    TRACK_MODE_TYPE TrackMode;
} RAW_READ_INFO, *PRAW_READ_INFO;

#define RAW_SECTOR_SIZE 2352

#endif  /* _LIBCUEIFY_WIN32_H */
