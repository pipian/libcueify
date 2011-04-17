/* charsets.h - Header for (CD-Text) text codec functions.
 *
 * Copyright (c) 2011 Ian Jacobi
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

#ifndef _7604DRIVERS_CHARSETS_H
#define _7604DRIVERS_CHARSETS_H

/** Allocate and populate a character buffer with the UTF-8
 *  translation of a string encoded with the ISO-8859-1 codec.
 *
 * @param szLatin1 A pointer to the string containing ISO-8859-1 data.
 * @param iSize The number of characters to convert.  If -1,
 *              characters will be translated until a null terminator
 *              or invalid character is found.
 * @return A newly allocated buffer containing a UTF-8 encoded version
 *         of the data in szLatin1.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
char *ConvertLatin1(char *szLatin1, int iSize);

/** Allocated and populate a character buffer with the UTF-8
 *  translation of a string encoded using the Music Shift-JIS codec
 *  (assuming big-endian encoding).
 *
 * @param szMSJIS A pointer to the string containing MS-JIS data.
 * @param iSize The number of wide characters to convert.  If -1,
 *              characters will be translated until a null terminator
 *              or invalid character is found.
 * @return A newly allocated buffer containing a UTF-8 encoded version
 *         of the data in szMSJIS.  If the buffer could not be
 *         allocated, NULL will be returned.  This buffer must be
 *         freed.
 */
char *ConvertMSJIS(char *szLatin1, int iSize);

#endif
