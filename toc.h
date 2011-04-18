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

#define IOCTL_CDROM_READ_Q_CHANNEL \
    CTL_CODE(IOCTL_CDROM_BASE, 0x000B, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_READ_TOC_EX \
    CTL_CODE(IOCTL_CDROM_BASE, 0x0015, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_SEEK_AUDIO_MSF \
    CTL_CODE(IOCTL_CDROM_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)

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
#define CDROM_CD_TEXT_PACK_CLOSED     0x8d
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
  CDROM_TOC_FULL_TOC_DATA_BLOCK  Descriptors[0];
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

/* CDROM_AUDIO_CONTROL.LbaFormat constants */
#define AUDIO_WITH_PREEMPHASIS            0x1
#define DIGITAL_COPY_PERMITTED            0x2
#define AUDIO_DATA_TRACK                  0x4
#define TWO_FOUR_CHANNEL_AUDIO            0x8

struct TrackIndex {
    UCHAR M;
    UCHAR S;
    UCHAR F;
};

struct CDTextTOC {
    unsigned char firstTrack; /** The first track in this CD-Text TOC. */
    unsigned char lastTrack;  /** The last track in this CD-Text TOC. */
    /**
     * Pointers to all track offsets in this CD-Text TOC. Index 0
     * contains the leadout.
     */
    struct TrackIndex trackPointers[MAXIMUM_NUMBER_TRACKS];
};

/** Data for a single TOC interval in CD-Text. */
struct CDTextTOCInterval {
    unsigned char priorityNumber; /** Priority number of this interval. */
    unsigned char numIntervals;   /** Number of intervals for a track. */
    struct TrackIndex start;      /** Start index of this interval. */
    struct TrackIndex end;        /** End index of this interval. */
};

struct CDTextTOC2 {
    int iIntervals;        /** The number of track interval structs. */
    /** The track interval structs. */
    struct CDTextTOCInterval *intervals;
};

#define CDROM_CD_TEXT_CHARSET_ISO8859_1 0x00
#define CDROM_CD_TEXT_CHARSET_ASCII     0x01
#define CDROM_CD_TEXT_CHARSET_MSJIS     0x80
#define CDROM_CD_TEXT_CHARSET_KOREAN    0x81
#define CDROM_CD_TEXT_CHARSET_CHINESE   0x82

enum {
    CDROM_CD_TEXT_LANG_UNKNOWN  = 0x00,
    CDROM_CD_TEXT_LANG_ALBANIAN,
    CDROM_CD_TEXT_LANG_BRETON,
    CDROM_CD_TEXT_LANG_CATALAN,
    CDROM_CD_TEXT_LANG_CROATIAN,
    CDROM_CD_TEXT_LANG_WELSH,
    CDROM_CD_TEXT_LANG_CZECH,
    CDROM_CD_TEXT_LANG_DANISH,
    CDROM_CD_TEXT_LANG_GERMAN,
    CDROM_CD_TEXT_LANG_ENGLISH,
    CDROM_CD_TEXT_LANG_SPANISH,
    CDROM_CD_TEXT_LANG_ESPERANTO,
    CDROM_CD_TEXT_LANG_ESTONIAN,
    CDROM_CD_TEXT_LANG_BASQUE,
    CDROM_CD_TEXT_LANG_FAROESE,
    CDROM_CD_TEXT_LANG_FRENCH,
    CDROM_CD_TEXT_LANG_FRISIAN,
    CDROM_CD_TEXT_LANG_IRISH,
    CDROM_CD_TEXT_LANG_GAELIC,
    CDROM_CD_TEXT_LANG_GALICIAN,
    CDROM_CD_TEXT_LANG_ICELANDIC,
    CDROM_CD_TEXT_LANG_ITALIAN,
    CDROM_CD_TEXT_LANG_SAMI,
    CDROM_CD_TEXT_LANG_LATIN,
    CDROM_CD_TEXT_LANG_LATVIAN,
    CDROM_CD_TEXT_LANG_LUXEMBOURGISH,
    CDROM_CD_TEXT_LANG_LITHUANIAN,
    CDROM_CD_TEXT_LANG_HUNGARIAN,
    CDROM_CD_TEXT_LANG_MALTESE,
    CDROM_CD_TEXT_LANG_DUTCH,
    CDROM_CD_TEXT_LANG_NORWEGIAN,
    CDROM_CD_TEXT_LANG_OCCITAN,
    CDROM_CD_TEXT_LANG_POLISH,
    CDROM_CD_TEXT_LANG_PORTUGUESE,
    CDROM_CD_TEXT_LANG_ROMANIAN,
    CDROM_CD_TEXT_LANG_ROMANSH,
    CDROM_CD_TEXT_LANG_SERBIAN,
    CDROM_CD_TEXT_LANG_SLOVAK,
    CDROM_CD_TEXT_LANG_SLOVENIAN,
    CDROM_CD_TEXT_LANG_FINNISH,
    CDROM_CD_TEXT_LANG_SWEDISH,
    CDROM_CD_TEXT_LANG_TURKISH,
    CDROM_CD_TEXT_LANG_FLEMISH,
    CDROM_CD_TEXT_LANG_WALLOON,
    CDROM_CD_TEXT_LANG_ZULU = 0x45,
    CDROM_CD_TEXT_LANG_VIETNAMESE,
    CDROM_CD_TEXT_LANG_UZBEK,
    CDROM_CD_TEXT_LANG_URDU,
    CDROM_CD_TEXT_LANG_UKRAINIAN,
    CDROM_CD_TEXT_LANG_THAI,
    CDROM_CD_TEXT_LANG_TELUGU,
    CDROM_CD_TEXT_LANG_TATAR,
    CDROM_CD_TEXT_LANG_TAMIL,
    CDROM_CD_TEXT_LANG_TAJIK,
    CDROM_CD_TEXT_LANG_SWAHILI,
    CDROM_CD_TEXT_LANG_SRANAN_TONGO,
    CDROM_CD_TEXT_LANG_SOMALI,
    CDROM_CD_TEXT_LANG_SINHALA,
    CDROM_CD_TEXT_LANG_SHONA,
    CDROM_CD_TEXT_LANG_SERBOCROAT,
    CDROM_CD_TEXT_LANG_RUTHENIAN,
    CDROM_CD_TEXT_LANG_RUSSIAN,
    CDROM_CD_TEXT_LANG_QUECHUA,
    CDROM_CD_TEXT_LANG_PUSHTU,
    CDROM_CD_TEXT_LANG_PUNJABI,
    CDROM_CD_TEXT_LANG_PERSIAN,
    CDROM_CD_TEXT_LANG_PAPIAMENTO,
    CDROM_CD_TEXT_LANG_ORIYA,
    CDROM_CD_TEXT_LANG_NEPALI,
    CDROM_CD_TEXT_LANG_NDEBELE,
    CDROM_CD_TEXT_LANG_MARATHI,
    CDROM_CD_TEXT_LANG_MOLDAVIAN,
    CDROM_CD_TEXT_LANG_MALAYSIAN,
    CDROM_CD_TEXT_LANG_MALAGASY,
    CDROM_CD_TEXT_LANG_MACEDONIAN,
    CDROM_CD_TEXT_LANG_LAO,
    CDROM_CD_TEXT_LANG_KOREAN,
    CDROM_CD_TEXT_LANG_KHMER,
    CDROM_CD_TEXT_LANG_KAZAKH,
    CDROM_CD_TEXT_LANG_KANNADA,
    CDROM_CD_TEXT_LANG_JAPANESE,
    CDROM_CD_TEXT_LANG_INDONESIAN,
    CDROM_CD_TEXT_LANG_HINDI,
    CDROM_CD_TEXT_LANG_HEBREW,
    CDROM_CD_TEXT_LANG_HAUSA,
    CDROM_CD_TEXT_LANG_GUARANI,
    CDROM_CD_TEXT_LANG_GUJARATI,
    CDROM_CD_TEXT_LANG_GREEK,
    CDROM_CD_TEXT_LANG_GEORGIAN,
    CDROM_CD_TEXT_LANG_FULAH,
    CDROM_CD_TEXT_LANG_DARI,
    CDROM_CD_TEXT_LANG_CHUVASH,
    CDROM_CD_TEXT_LANG_CHINESE,
    CDROM_CD_TEXT_LANG_BURMESE,
    CDROM_CD_TEXT_LANG_BULGARIAN,
    CDROM_CD_TEXT_LANG_BENGALI,
    CDROM_CD_TEXT_LANG_BELARUSIAN,
    CDROM_CD_TEXT_LANG_BAMBARA,
    CDROM_CD_TEXT_LANG_AZERBAIJANI,
    CDROM_CD_TEXT_LANG_ASSAMESE,
    CDROM_CD_TEXT_LANG_ARMENIAN,
    CDROM_CD_TEXT_LANG_ARABIC,
    CDROM_CD_TEXT_LANG_AMHARIC
};

/**
 * Data for a single CD-Text language block. For arrays storing both
 * disc and track information, index 0 contains disc information, and
 * subsequent indices contain track information.
 */
struct CDTextBlock {
    BOOL bValid;            /** TRUE if this block is a valid block. */
    unsigned char charset;  /** The character set used to encode this block. */
    unsigned char language; /** The language of the text in this block. */
    int iTracks;            /** The number of tracks in this block. */
    BOOL bMode2;            /** Is there CD-Text in MODE 2? */
    BOOL bProgramCopyright; /** Is there copyrighted CD-Text in MODE 2? */
    BOOL bMessageCopyright; /** Are the MESSAGEs copyrighted? */
    BOOL bNameCopyright;    /** Are names (PERFORMER, etc.) copyrighted? */
    BOOL bTitleCopyright;   /** Are titles copyrighted? */
    char **titles;          /** Disc and track titles. */
    char **performers;      /** Disc and track performer names. */
    char **songwriters;     /** Disc and track songwriter names. */
    char **composers;       /** Disc and track composer names. */
    char **arrangers;       /** Disc and track arranger names. */
    char **messages;        /** Disc and track messages. */
    char **upc_ean_isrcs;   /** Disc UPC/EAN and track ISRCs. */
    char *discID;          /** Disc ID. */
    int genreCode;          /** CD-Text genre code. */
    char *genreName;        /** CD-Text supplementary genre name. */
};

struct CDText {
    struct CDTextBlock blocks[8]; /** The blocks in the CD-Text. */
    struct CDTextTOC tocInfo;     /** The TOC info in the CD-Text. */
    struct CDTextTOC2 tocInfo2;   /** The 2nd TOC info in the CD-Text. */
};

struct TrackIndices {
    int iIndices;
    BOOL bHasPregap;
    struct TrackIndex *indices;
};

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

/** Read the contents of the CD-ROM TOC into a CDROM_TOC_FULL_TOC_DATA
 *  value.  The returned pointer should be freed using free().
 *
 * @param hDevice A handle to the drive to read the TOC data from.
 * @return An allocated CDROM_TOC_FULL_TOC_DATA structure if the command
 *         succeeded, or NULL if it failed.
 */
CDROM_TOC_FULL_TOC_DATA *ReadFullTOC(HANDLE hDevice);

/** Read the contents of the CD-Text into a CDROM_TOC_CD_TEXT_DATA
 *  value.  The returned pointer should be freed using free().
 *
 * @param hDevice A handle to the drive to read the CD-Text data from.
 * @return An allocated CDROM_TOC_CD_TEXT_DATA structure if the command
 *         succeeded, or NULL if it failed.
 */
CDROM_TOC_CD_TEXT_DATA *ReadCDText(HANDLE hDevice);

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

/** Read the current position into a SUB_Q_CHANNEL_DATA variable provided.
 *
 * @param hDevice A handle to the drive to read the current position from.
 * @param iTrack The track to read the current position from.
 * @param iMinute The absolute minute to seek to prior to getting position.
 * @param iSecond The absolute second to seek to prior to getting position.
 * @param iFrame The absolute frame to seek to prior to getting position.
 * @param data A pointer to the SUB_Q_CHANNEL_DATA to populate.
 * @return TRUE if the command succeeded.
 */
BOOL ReadCurrentPosition(HANDLE hDevice, int iTrack,
			 int iMinute, int iSecond, int iFrame,
			 SUB_Q_CHANNEL_DATA *data);

/** Parse a CDROM_TOC_CD_TEXT_DATA struct into a meaningful CDText struct.
 *
 * @param cdtext The CDROM_TOC_CD_TEXT_DATA struct to parse.
 * @return An allocated CDText structure containing the parsed contents of
 *         cdtext if successful (this should be freed with FreeCDText()),
 *         or NULL if not.
 */
struct CDText *ParseCDText(CDROM_TOC_CD_TEXT_DATA *cdtext);

/** Free an allocated CDText struct.
 *
 * @param cdtextData The CDText struct to free.
 */
void FreeCDText(struct CDText *cdtextData);

/** Detect indices on the given track.
 *
 * @param hDevice A handle to the drive to detect indices on.
 * @param toc The TOC of the current CD.
 * @param iTrack The track to detect indices on.
 * @param indices The TrackIndices struct to populate.
 * @return TRUE if the detection succeeded.
 */
BOOL DetectTrackIndices(HANDLE hDevice, CDROM_TOC *toc, int iTrack,
			struct TrackIndices *indices);

#endif
