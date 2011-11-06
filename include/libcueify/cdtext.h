/* cdtext.h - Header for CD-ROM functions which read the CD-Text data
 * of a disc.
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

#ifndef _LIBCUEIFY_CDTEXT_H
#define _LIBCUEIFY_CDTEXT_H

#include <libcueify/device.h>
#include <libcueify/constants.h>
#include <libcueify/types.h>

/**
 * A transparent handle for the CD-Text data of an audio CD.
 *
 * This is returned by cueify_cdtext_new() and is passed as the first
 * parameter to all cueify_cdtext_*() functions.
 */
typedef void *cueify_cdtext;


/**
 * A transparent handle for a block of CD-Text data of an audio CD.
 *
 * This is returned by cueify_cdtext_get_block() and is passed as the first
 * parameter to all cueify_cdtext_block_*() functions.
 */
typedef void *cueify_cdtext_block;


/**
 * Create a new CD-Text instance. The instance is created with no
 * data, and should be populated using cueify_device_read_cdtext(),
 * cueify_cdtext_deserialize(), or the various
 * cueify_cdtext_set_*() functions.
 *
 * @return NULL if there was an error allocating memory, else the new
 *         CD-Text data
 */
cueify_cdtext *cueify_cdtext_new();


/**
 * Read the CD-Text data of the disc in the optical disc device
 * associated with a device handle.
 *
 * @pre { d != NULL, t != NULL }
 * @param d an opened device handle
 * @param t a CD-Text instance to populate
 * @return CUEIFY_OK if the CD-Text data was successfully read;
 *         otherwise an error code is returned
 */
int cueify_device_read_cdtext(cueify_device *d, cueify_cdtext *t);


/**
 * Deserialize a CD-Text instance previously serialized with
 * cueify_cdtext_serialize().
 *
 * @note This serialization is, in principle, the same as that on the
 *       disc itself.
 *
 * @pre { t != NULL, buffer != NULL }
 * @param t a CD-Text instance to populate
 * @param buffer a pointer to the serialized CD-Text data
 * @param size the size of the buffer
 * @return CUEIFY_OK if the CD-Text instance was successfully
 *         deserialized; otherwise an error code is returned
 */
int cueify_cdtext_deserialize(cueify_cdtext *t, uint8_t *buffer, size_t size);


/**
 * Serialize a CD-Text instance for later deserialization with
 * cueify_cdtext_deserialize().
 *
 * @note This serialization is, in principle, the same as that on the
 *       disc itself.
 *
 * @pre { t != NULL, size != NULL }
 * @param t a CD-Text instance to serialize
 * @param buffer a pointer to a location to serialize data to, or NULL
 *               to determine the optimal size of such a buffer
 * @param size a pointer to the size of the buffer. When called, the
 *             size must contain the maximum number of bytes that may
 *             be stored in buffer. When this function is complete,
 *             the pointer will contain the actual number of bytes
 *             serialized, or if buffer is NULL, the number of bytes
 *             needed to fully serialize the CD-Text instance.
 * @return CUEIFY_OK if the CD-Text instance was successfully
 *         serialized; otherwise an error code is returned
 */
int cueify_cdtext_serialize(cueify_cdtext *t, uint8_t *buffer, size_t *size);


/**
 * Free a CD-Text instance. Deletes the object pointed to by t.
 *
 * @pre { t != NULL }
 * @param t a cueify_cdtext object created by cueify_cdtext_new()
 */
void cueify_cdtext_free(cueify_cdtext *t);


/**
 * Get the number of the first track in the TOC in a CD-Text instance.
 *
 * @pre { t != NULL }
 * @param t a CD-Text instance
 * @return the number of the first track in the TOC in t
 */
uint8_t cueify_cdtext_get_toc_first_track(cueify_cdtext *t);


/**
 * Get the number of the last track in the TOC in a CD-Text instance.
 *
 * @pre { t != NULL }
 * @param t a CD-Text instance
 * @return the number of the last track in the TOC in t
 */
uint8_t cueify_cdtext_get_toc_last_track(cueify_cdtext *t);


/**
 * Get the time of the start address of a track in the TOC in a
 * CD-Text instance.
 *
 * @pre { t != NULL,
 *        cueify_cdtext_get_toc_first_track(t) <= track,
 *        track <= cueify_cdtext_get_toc_last_track(t) }
 * @param t a CD-Text instance
 * @param track the number of the track for which the time of the
 *              start address should be returned
 * @return the time of the start address of track number track in the TOC in t
 */
cueify_msf_t cueify_cdtext_get_toc_track_offset(cueify_cdtext *t,
						uint8_t track);


/**
 * Get the number of track intervals for a track in the TOC in a
 * CD-Text instance.
 *
 * @pre { t != NULL,
 *        cueify_cdtext_get_toc_first_track(t) <= track,
 *        track <= cueify_cdtext_get_toc_last_track(t) }
 * @param t a CD-Text instance
 * @param track the number of the track for which the number of track
 *              intervals in the TOC
 * @return the number of track intervals for track number track in the TOC in t
 */
uint8_t cueify_cdtext_get_toc_num_track_intervals(cueify_cdtext *t,
						  uint8_t track);


/**
 * Get the start address of a track interval in the TOC in a CD-Text instance.
 *
 * @pre { t != NULL,
 *        cueify_cdtext_get_toc_first_track(t) <= track,
 *        track <= cueify_cdtext_get_toc_last_track(t),
 *        0 <= interval < cueify_cdtext_get_toc_num_track_intervals(t, track) }
 * @param t a CD-Text instance
 * @param track the number of the track to which the interval belongs
 * @param interval the index of the interval in the track for which
 *                 the start address should be returned
 * @return the start address of the track interval number interval in
 *         track number track in the TOC in t
 */
cueify_msf_t cueify_cdtext_get_toc_track_interval_start(cueify_cdtext *t,
							uint8_t track,
							uint8_t interval);


/**
 * Get the end address of a track interval in the TOC in a CD-Text instance.
 *
 * @pre { t != NULL,
 *        cueify_cdtext_get_toc_first_track(t) <= track,
 *        track <= cueify_cdtext_get_toc_last_track(t),
 *        0 <= interval < cueify_cdtext_get_toc_num_track_intervals(t, track) }
 * @param t a CD-Text instance
 * @param track the number of the track to which the interval belongs
 * @param interval the index of the interval in the track for which
 *                 the end address should be returned
 * @return the end address of the track interval number interval in
 *         track number track in the TOC in t
 */
cueify_msf_t cueify_cdtext_get_toc_track_interval_end(cueify_cdtext *t,
						      uint8_t track,
						      uint8_t interval);


/** CD-Text uses the ISO-8859-1 encoding. */
#define CUEIFY_CDTEXT_CHARSET_ISO8859_1  0x00
/** CD-Text uses the ASCII encoding. */
#define CUEIFY_CDTEXT_CHARSET_ASCII      0x01
/** CD-Text uses the MS-JIS encoding. */
#define CUEIFY_CDTEXT_CHARSET_MSJIS      0x80
/** CD-Text uses a (undefined) Korean encoding. */
#define CUEIFY_CDTEXT_CHARSET_KOREAN     0x81
/** CD-Text uses a (undefined) Chinese encoding. */
#define CUEIFY_CDTEXT_CHARSET_CHINESE    0x82


/** Language identifiers of CD-Text blocks */
enum {
    CUEIFY_CDTEXT_LANG_UNKNOWN  = 0x00,
    CUEIFY_CDTEXT_LANG_ALBANIAN,
    CUEIFY_CDTEXT_LANG_BRETON,
    CUEIFY_CDTEXT_LANG_CATALAN,
    CUEIFY_CDTEXT_LANG_CROATIAN,
    CUEIFY_CDTEXT_LANG_WELSH,
    CUEIFY_CDTEXT_LANG_CZECH,
    CUEIFY_CDTEXT_LANG_DANISH,
    CUEIFY_CDTEXT_LANG_GERMAN,
    CUEIFY_CDTEXT_LANG_ENGLISH,
    CUEIFY_CDTEXT_LANG_SPANISH,
    CUEIFY_CDTEXT_LANG_ESPERANTO,
    CUEIFY_CDTEXT_LANG_ESTONIAN,
    CUEIFY_CDTEXT_LANG_BASQUE,
    CUEIFY_CDTEXT_LANG_FAROESE,
    CUEIFY_CDTEXT_LANG_FRENCH,
    CUEIFY_CDTEXT_LANG_FRISIAN,
    CUEIFY_CDTEXT_LANG_IRISH,
    CUEIFY_CDTEXT_LANG_GAELIC,
    CUEIFY_CDTEXT_LANG_GALICIAN,
    CUEIFY_CDTEXT_LANG_ICELANDIC,
    CUEIFY_CDTEXT_LANG_ITALIAN,
    CUEIFY_CDTEXT_LANG_SAMI,
    CUEIFY_CDTEXT_LANG_LATIN,
    CUEIFY_CDTEXT_LANG_LATVIAN,
    CUEIFY_CDTEXT_LANG_LUXEMBOURGISH,
    CUEIFY_CDTEXT_LANG_LITHUANIAN,
    CUEIFY_CDTEXT_LANG_HUNGARIAN,
    CUEIFY_CDTEXT_LANG_MALTESE,
    CUEIFY_CDTEXT_LANG_DUTCH,
    CUEIFY_CDTEXT_LANG_NORWEGIAN,
    CUEIFY_CDTEXT_LANG_OCCITAN,
    CUEIFY_CDTEXT_LANG_POLISH,
    CUEIFY_CDTEXT_LANG_PORTUGUESE,
    CUEIFY_CDTEXT_LANG_ROMANIAN,
    CUEIFY_CDTEXT_LANG_ROMANSH,
    CUEIFY_CDTEXT_LANG_SERBIAN,
    CUEIFY_CDTEXT_LANG_SLOVAK,
    CUEIFY_CDTEXT_LANG_SLOVENIAN,
    CUEIFY_CDTEXT_LANG_FINNISH,
    CUEIFY_CDTEXT_LANG_SWEDISH,
    CUEIFY_CDTEXT_LANG_TURKISH,
    CUEIFY_CDTEXT_LANG_FLEMISH,
    CUEIFY_CDTEXT_LANG_WALLOON,
    CUEIFY_CDTEXT_LANG_ZULU = 0x45,
    CUEIFY_CDTEXT_LANG_VIETNAMESE,
    CUEIFY_CDTEXT_LANG_UZBEK,
    CUEIFY_CDTEXT_LANG_URDU,
    CUEIFY_CDTEXT_LANG_UKRAINIAN,
    CUEIFY_CDTEXT_LANG_THAI,
    CUEIFY_CDTEXT_LANG_TELUGU,
    CUEIFY_CDTEXT_LANG_TATAR,
    CUEIFY_CDTEXT_LANG_TAMIL,
    CUEIFY_CDTEXT_LANG_TAJIK,
    CUEIFY_CDTEXT_LANG_SWAHILI,
    CUEIFY_CDTEXT_LANG_SRANAN_TONGO,
    CUEIFY_CDTEXT_LANG_SOMALI,
    CUEIFY_CDTEXT_LANG_SINHALA,
    CUEIFY_CDTEXT_LANG_SHONA,
    CUEIFY_CDTEXT_LANG_SERBOCROAT,
    CUEIFY_CDTEXT_LANG_RUTHENIAN,
    CUEIFY_CDTEXT_LANG_RUSSIAN,
    CUEIFY_CDTEXT_LANG_QUECHUA,
    CUEIFY_CDTEXT_LANG_PUSHTU,
    CUEIFY_CDTEXT_LANG_PUNJABI,
    CUEIFY_CDTEXT_LANG_PERSIAN,
    CUEIFY_CDTEXT_LANG_PAPIAMENTO,
    CUEIFY_CDTEXT_LANG_ORIYA,
    CUEIFY_CDTEXT_LANG_NEPALI,
    CUEIFY_CDTEXT_LANG_NDEBELE,
    CUEIFY_CDTEXT_LANG_MARATHI,
    CUEIFY_CDTEXT_LANG_MOLDAVIAN,
    CUEIFY_CDTEXT_LANG_MALAYSIAN,
    CUEIFY_CDTEXT_LANG_MALAGASY,
    CUEIFY_CDTEXT_LANG_MACEDONIAN,
    CUEIFY_CDTEXT_LANG_LAO,
    CUEIFY_CDTEXT_LANG_KOREAN,
    CUEIFY_CDTEXT_LANG_KHMER,
    CUEIFY_CDTEXT_LANG_KAZAKH,
    CUEIFY_CDTEXT_LANG_KANNADA,
    CUEIFY_CDTEXT_LANG_JAPANESE,
    CUEIFY_CDTEXT_LANG_INDONESIAN,
    CUEIFY_CDTEXT_LANG_HINDI,
    CUEIFY_CDTEXT_LANG_HEBREW,
    CUEIFY_CDTEXT_LANG_HAUSA,
    CUEIFY_CDTEXT_LANG_GUARANI,
    CUEIFY_CDTEXT_LANG_GUJARATI,
    CUEIFY_CDTEXT_LANG_GREEK,
    CUEIFY_CDTEXT_LANG_GEORGIAN,
    CUEIFY_CDTEXT_LANG_FULAH,
    CUEIFY_CDTEXT_LANG_DARI,
    CUEIFY_CDTEXT_LANG_CHUVASH,
    CUEIFY_CDTEXT_LANG_CHINESE,
    CUEIFY_CDTEXT_LANG_BURMESE,
    CUEIFY_CDTEXT_LANG_BULGARIAN,
    CUEIFY_CDTEXT_LANG_BENGALI,
    CUEIFY_CDTEXT_LANG_BELARUSIAN,
    CUEIFY_CDTEXT_LANG_BAMBARA,
    CUEIFY_CDTEXT_LANG_AZERBAIJANI,
    CUEIFY_CDTEXT_LANG_ASSAMESE,
    CUEIFY_CDTEXT_LANG_ARMENIAN,
    CUEIFY_CDTEXT_LANG_ARABIC,
    CUEIFY_CDTEXT_LANG_AMHARIC
};


/**
 * Get the number of the blocks in a CD-Text instance.
 *
 * @pre { t != NULL }
 * @param t a CD-Text instance
 * @return the number of blocks in t
 */
uint8_t cueify_cdtext_get_num_blocks(cueify_cdtext *t);


/**
 * Get a block from a CD-Text instance.
 *
 * @pre { t != NULL, 0 <= block < cueify_cdtext_get_num_blocks(t) }
 * @param t a CD-Text instance
 * @param block the number of the block to retrieve
 * @return an instance of block number CD-Text block in t
 */
cueify_cdtext_block *cueify_cdtext_get_block(cueify_cdtext *t, uint8_t block);


/**
 * Get the number of the first track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return the number of the first track in b
 */
uint8_t cueify_cdtext_block_get_first_track(cueify_cdtext_block *b);


/**
 * Get the number of the last track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return the number of the last track in b
 */
uint8_t cueify_cdtext_block_get_last_track(cueify_cdtext_block *b);


/**
 * Get the character set used to encode data in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return the character set used to encode the metadata in b
 */
uint8_t cueify_cdtext_block_get_charset(cueify_cdtext_block *b);


/**
 * Get the language used to encode data in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return the language used to encode the metadata in b
 */
uint8_t cueify_cdtext_block_get_language(cueify_cdtext_block *b);


/**
 * Get whether or not CD-Text data is available in program data on this disc.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return TRUE if the data in b is available in Mode 2 packets in
 *         program data on this disc, otherwise FALSE
 */
uint8_t cueify_cdtext_block_has_program_data(cueify_cdtext_block *b);


/**
 * Get whether or not CD-Text data in the program track is copyrighted.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return TRUE if the data in b is copyrighted in the program data,
 *         otherwise FALSE
 */
uint8_t cueify_cdtext_block_has_program_copyright(cueify_cdtext_block *b);


/**
 * Get whether or not the message in a CD-Text block is copyrighted.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return TRUE if the message data in b is copyrighted, otherwise FALSE
 */
uint8_t cueify_cdtext_block_has_message_copyright(cueify_cdtext_block *b);


/**
 * Get whether or not the artist names in a CD-Text block are copyrighted.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return TRUE if the artist names in b is copyrighted, otherwise FALSE
 */
uint8_t cueify_cdtext_block_has_name_copyright(cueify_cdtext_block *b);


/**
 * Get whether or not the titles in a CD-Text block are copyrighted.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return TRUE if the titles in b are copyrighted, otherwise FALSE
 */
uint8_t cueify_cdtext_block_has_title_copyright(cueify_cdtext_block *b);


/** Track number to represent the entire album rather than a single track. */
#define CUEIFY_CDTEXT_ALBUM  0x00


/**
 * Get the title of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the title for (or
 *        CUEIFY_CDTEXT_ALBUM for the title of the album)
 * @return NULL if no title has been set for the track, otherwise the title
 *         in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_title(cueify_cdtext_block *b,
					  uint8_t track);


/**
 * Get the performer of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the performer for (or
 *        CUEIFY_CDTEXT_ALBUM for the performer of the album)
 * @return NULL if no performer has been set for the track, otherwise
 *         the name of the performer in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_performer(cueify_cdtext_block *b,
					      uint8_t track);


/**
 * Get the songwriter of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the songwriter for (or
 *        CUEIFY_CDTEXT_ALBUM for the songwriter of the album)
 * @return NULL if no songwriter has been set for the track, otherwise
 *         the name of the songwriter in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_songwriter(cueify_cdtext_block *b,
					       uint8_t track);


/**
 * Get the composer of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the composer for (or
 *        CUEIFY_CDTEXT_ALBUM for the composer of the album)
 * @return NULL if no composer has been set for the track, otherwise
 *         the name of the composer in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_composer(cueify_cdtext_block *b,
					     uint8_t track);


/**
 * Get the arranger of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the arranger for (or
 *        CUEIFY_CDTEXT_ALBUM for the arranger of the album)
 * @return NULL if no arranger has been set for the track, otherwise
 *         the name of the arranger in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_arranger(cueify_cdtext_block *b,
					     uint8_t track);


/**
 * Get the message of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the message for (or
 *        CUEIFY_CDTEXT_ALBUM for the message of the album)
 * @return NULL if no message has been set for the track, otherwise
 *         the message in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_message(cueify_cdtext_block *b,
					    uint8_t track);


/**
 * Get the message of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the message for (or
 *        CUEIFY_CDTEXT_ALBUM for the message of the album)
 * @return NULL if no message has been set for the track, otherwise
 *         the message in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_message(cueify_cdtext_block *b,
					    uint8_t track);


/**
 * Get the private data of a track in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the private data for (or
 *        CUEIFY_CDTEXT_ALBUM for the message of the album)
 * @return NULL if no private data has been set for the track,
 *         otherwise the private data in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_private(cueify_cdtext_block *b,
					    uint8_t track);


/**
 * Get the ISRC of a track or UPC of the album in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @param track the number of the track to retrieve the ISRC for (or
 *        CUEIFY_CDTEXT_ALBUM for the UPC of the album)
 * @return NULL if no ISRC/UPC has been set for the track, otherwise
 *         the name ISRC/UPC in UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_upc_isrc(cueify_cdtext_block *b,
					     uint8_t track);


/**
 * Get the discid in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return NULL if no discid has been set.  Otherwise the discid in
 *         UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_discid(cueify_cdtext_block *b);


/**
 * Get the genre code in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return 0 if no genre code has been set.  Otherwise the code.
 */
uint16_t cueify_cdtext_block_get_genre_code(cueify_cdtext_block *b);


/**
 * Get the genre name in a CD-Text block.
 *
 * @pre { b != NULL }
 * @param b a CD-Text block instance
 * @return NULL if no genre name has been set.  Otherwise the name in
 *         UTF-8 encoding.
 */
const char *cueify_cdtext_block_get_genre_name(cueify_cdtext_block *b);

#endif /* _LIBCUEIFY_CDTEXT_H */
