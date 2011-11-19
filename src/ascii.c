/* ascii.c - ASCII to UTF-8 text codec functions.
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

#include <string.h>
#include <stdlib.h>

#include "charsets.h"

size_t ascii_byte_count(char *utf8) {
    size_t size = 0;
    uint8_t *bp = (uint8_t *)utf8;
    uint32_t character;

    while (*bp != 0) {
	character = 0;
	if (*bp <= 0x7F) {
	    /* 1-byte codepoint */
	    character = *bp++;
	} else if (*bp <= 0xBF) {
	    /* Invalid codepoint */
	    character = '?';
	    bp++;
	} else if (*bp <= 0xDF) {
	    /* 2-byte codepoint */
	    character  = (*bp++ & 0x1F) << 6;
	    character |= (*bp++ & 0x3F);
	    if (character <= 0x7F) {
		character = '?';
	    }
	} else if (*bp <= 0xEF) {
	    /* 3-byte codepoint */
	    character  = (*bp++ & 0x0F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xF7) {
	    /* 4-byte codepoint */
	    character  = (*bp++ & 0x07) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xFB) {
	    /* 5-byte codepoint */
	    character  = (*bp++ & 0x03) << 24;
	    character |= (*bp++ & 0x3F) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xFD) {
	    /* 6-byte codepoint */
	    character  = (*bp++ & 0x01) << 30;
	    character |= (*bp++ & 0x3F) << 24;
	    character |= (*bp++ & 0x3F) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	}
	if (character > 0x7F) {
	    character = '?';
	}
	size++;
    }
    /* Include the terminator */
    size++;

    return size;
}  /* ascii_byte_count */


uint8_t *utf8_to_ascii(char *utf8, size_t *size) {
    uint8_t *output = NULL, *output_ptr = NULL;
    uint8_t *bp = (uint8_t *)utf8;
    uint32_t character;

    *size = strlen(utf8) + 1;
    output = malloc(*size);
    if (output == NULL) {
	return NULL;
    }
    output_ptr = output;

    while (*bp != 0) {
	character = 0;
	if (*bp <= 0x7F) {
	    /* 1-byte codepoint */
	    character = *bp++;
	} else if (*bp <= 0xBF) {
	    /* Invalid codepoint */
	    character = '?';
	    bp++;
	} else if (*bp <= 0xDF) {
	    /* 2-byte codepoint */
	    character  = (*bp++ & 0x1F) << 6;
	    character |= (*bp++ & 0x3F);
	    if (character <= 0x7F) {
		character = '?';
	    }
	} else if (*bp <= 0xEF) {
	    /* 3-byte codepoint */
	    character  = (*bp++ & 0x0F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xF7) {
	    /* 4-byte codepoint */
	    character  = (*bp++ & 0x07) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xFB) {
	    /* 5-byte codepoint */
	    character  = (*bp++ & 0x03) << 24;
	    character |= (*bp++ & 0x3F) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	} else if (*bp <= 0xFD) {
	    /* 6-byte codepoint */
	    character  = (*bp++ & 0x01) << 30;
	    character |= (*bp++ & 0x3F) << 24;
	    character |= (*bp++ & 0x3F) << 18;
	    character |= (*bp++ & 0x3F) << 12;
	    character |= (*bp++ & 0x3F) << 6;
	    character |= (*bp++ & 0x3F);
	}
	if (character > 0x7F) {
	    character = '?';
	}
	*output_ptr++ = (uint8_t)character;
    }
    /* Include the terminator */
    *output_ptr = '\0';
    *size = (output_ptr - output) + 1;

    return output;
}  /* utf8_to_ascii */
