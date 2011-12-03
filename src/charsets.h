/* charsets.h - Header for (CD-Text) text codec functions.
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

#ifndef _CUEIFY_CHARSETS_H
#define _CUEIFY_CHARSETS_H

#include <cueify/types.h>

/** Multi-byte codepoint data for encoding. */
struct multibyte_codepoint {
    /** Has an encoding of the codepoint */
    int has_encoding;
    /** Encoding of the codepoint */
    uint8_t value[2];
    /** Master table for successor characters */
    const struct multibyte_codepoint * const * const *successor_master_table;
};


/** Allocate and populate a character buffer with the UTF-8
 *  translation of a string encoded with the ISO-8859-1 codec.
 *
 * @param latin1 a pointer to the string containing ISO-8859-1 data.
 * @param size the number of characters to convert.  If -1,
 *             characters will be translated until a null terminator
 *             or invalid character is found.
 * @return a newly allocated buffer containing a UTF-8 encoded version
 *         of the data in latin.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
char *latin1_to_utf8(uint8_t *latin1, int size);


/** Allocate and populate a character buffer with the UTF-8
 *  translation of a string encoded using the Music Shift-JIS codec
 *  (assuming big-endian encoding).
 *
 * @param msjis a pointer to the string containing MS-JIS data.
 * @param size the number of wide characters to convert.  If -1,
 *             characters will be translated until a null terminator
 *             or invalid character is found.
 * @return a newly allocated buffer containing a UTF-8 encoded version
 *         of the data in msjis.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
char *msjis_to_utf8(uint8_t *msjis, int size);


/** Get the number of bytes that would be needed to encode a UTF-8
 *  string in ASCII.
 *
 * @note Characters which cannot be encoded in ASCII will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a UTF-8 string to re-encode as ASCII
 * @return the number of bytes that would be needed to encode utf8 in
 *         ASCII (including the terminating NULL character)
 */
size_t ascii_byte_count(char *utf8);


/** Get the number of bytes that would be needed to encode a UTF-8
 *  string in ISO-8859-1.
 *
 * @note Characters which cannot be encoded in ISO-8859-1 will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a UTF-8 string to re-encode as ISO-8859-1
 * @return the number of bytes that would be needed to encode utf8 in
 *         ISO-8859-1 (including the terminating NULL character)
 */
size_t latin1_byte_count(char *utf8);


/** Get the number of bytes that would be needed to encode a UTF-8
 *  string in the Music Shift-JIS codec (assuming big-endian encoding).
 *
 * @note Characters which cannot be encoded in Shift-JIS will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a UTF-8 string to re-encode as Music Shift-JIS
 * @return the number of bytes that would be needed to encode utf8 in
 *         Music Shift-JIS (including the terminating NULL character)
 */
size_t msjis_byte_count(char *utf8);


/** Allocate and populate a character buffer with the ASCII
 *  translation of a string encoded using the UTF-8 codec.
 *
 * @note Characters which cannot be encoded in ASCII will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a pointer to the string containing UTF-8 data
 * @param size a pointer in which the number of bytes in the returned buffer
 *             will be stored
 * @return a newly allocated buffer containing an ASCII encoded version
 *         of the data in utf8.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
uint8_t *utf8_to_ascii(char *utf8, size_t *size);


/** Allocate and populate a character buffer with the ISO-8859-1
 *  translation of a string encoded using the UTF-8 codec.
 *
 * @note Characters which cannot be encoded in ISO-8859-1 will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a pointer to the string containing UTF-8 data
 * @param size a pointer in which the number of bytes in the returned buffer
 *             will be stored
 * @return a newly allocated buffer containing an ISO-8859-1 encoded version
 *         of the data in utf8.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
uint8_t *utf8_to_latin1(char *utf8, size_t *size);


/** Allocate and populate a character buffer with the Music Shift-JIS
 *  translation of a string encoded using the UTF-8 codec.
 *
 * @note Characters which cannot be encoded in Music Shift-JIS will be replaced
 *       with '?' before counting.
 *
 * @param utf8 a pointer to the string containing UTF-8 data
 * @param size a pointer in which the number of bytes in the returned buffer
 *             will be stored
 * @return a newly allocated buffer containing a Music Shift-JIS
 *         encoded version of the data in utf8.  If the buffer could
 *         not be allocated, NULL will be returned.  This buffer must
 *         be freed.
 */
uint8_t *utf8_to_msjis(char *utf8, size_t *size);

#endif
