/* msjis.c - Music Shift-JIS to UTF-8 text codec functions.
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

#include <string.h>
#include <stdlib.h>

#include "charsets.h"
#include "msjis_tables.h"

char *msjis_to_utf8(uint8_t *msjis, int size)
{
    int output_size = 0, i;
    uint8_t hi, lo;
    const char * const *table;
    const char *character;
    char *output = NULL, *output_ptr;

    if (size < 0) {
	/* Count output size until we find a terminator/bad char. */
	output = (char *)msjis;
	do {
	    hi = *output++;
	    lo = *output++;
	    table = master_table[(unsigned char)hi];
	    if (table == NULL) {
		/* Don't know this table! */
		break;
	    } else if (table[(unsigned char)lo][0] != '\0') {
		output_size += strlen(table[(unsigned char)lo]);
	    }
	} while (table[(unsigned char)lo] != '\0');
	/* And also include the terminator. */
	output_size++;
    } else {
	/* Convert exactly size (wide) characters. */
	output = (char *)msjis;
	for (i = 0; i < size; i++) {
	    hi = *output++;
	    lo = *output++;
	    table = master_table[(unsigned char)hi];
	    if (table == NULL) {
		/* Don't know this table! */
		output_size++;
	    } else if (table[(unsigned char)lo][0] == '\0') {
		output_size++;
	    } else {
		output_size += strlen(table[(unsigned char)lo]);
	    }
	}
	/* And also add a terminator. */
	output_size++;
    }
    
    /* Allocate space for the conversion... */
    output = calloc(output_size, 1);
    if (output == NULL) {
	return NULL;
    }
    output[--output_size] = '\0';

    /* Now do the conversion. */
    output_ptr = output;
    while (output_size > 0) {
	hi = *msjis++;
	lo = *msjis++;
	table = master_table[(unsigned char)hi];
	if (table == NULL) {
	    /* Don't know this table! */
	    *output_ptr++ = '\0';
	    output_size--;
	} else if (table[(unsigned char)lo][0] == '\0') {
	    *output_ptr++ = '\0';
	    output_size--;
	} else {
	    character = table[(unsigned char)lo];
	    strcpy(output_ptr, character);
	    output_ptr += strlen(character);
	    output_size -= strlen(character);
	}
    }

    return output;
}


size_t msjis_byte_count(char *utf8) {
    size_t size = 0;
    uint8_t *bp = (uint8_t *)utf8;
    uint8_t hi, mid, lo;
    uint32_t character;
    struct multibyte_codepoint codepoint;
    const struct multibyte_codepoint * const * const *master_table;
    const struct multibyte_codepoint * const *subtable;
    const struct multibyte_codepoint *table;

    master_table = reverse_master_table;
    while (*bp != 0) {
	character = 0;
	if (*bp <= 0x7F) {
	    /* 1-byte codepoint */
	    character = *bp;
	} else if (*bp <= 0xBF) {
	    /* Invalid codepoint */
	    character = '?';
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
	if (character > 0x10FFFF) {
	    character = '?';
	}

	/* Convert the character to MS-JIS (or wait for another
	 * character I guess) */
	/* TODO: What about successor characters (e.g. variation selectors)? */
	hi = (character >> 16) & 0xFF;
	mid = (character >> 8) & 0xFF;
	lo = character & 0xFF;
	if (master_table[hi] == NULL) {
	    /* No MS-JIS encoding. */
	    codepoint = reverse_table0000['?'];
	} else {
	    subtable = master_table[hi];

	    if (subtable[mid] == NULL) {
		/* No MS-JIS encoding. */
		codepoint = reverse_table0000['?'];
	    } else {
		table = subtable[mid];
		if (table[lo].has_encoding == 0 &&
		    table[lo].successor_master_table == NULL) {
		    /* No MS-JIS encoding. */
		    codepoint = reverse_table0000['?'];
		} else {
		    codepoint = table[lo];
		}
	    }
	}

	if (codepoint.successor_master_table) {
	    /* Check the successive character. */
	    master_table = codepoint.successor_master_table;
	} else {
	    size++;
	    /* Reset to look at the master table again. */
	    master_table = reverse_master_table;
	}
    }
    if (master_table != reverse_master_table) {
	/* In this case, we must have read one character with no successor. */
	size++;
    }
    /* Include the terminator */
    size++;

    return size * 2;  /* 2-byte character set */
}  /* msjis_byte_count */


uint8_t *utf8_to_msjis(char *utf8, size_t *size) {
    uint8_t *output = NULL, *output_ptr = NULL;
    uint8_t *bp = (uint8_t *)utf8;
    uint8_t hi, mid, lo;
    uint32_t character;
    struct multibyte_codepoint codepoint;
    const struct multibyte_codepoint * const * const *master_table;
    const struct multibyte_codepoint * const *subtable;
    const struct multibyte_codepoint *table;

    master_table = reverse_master_table;
    while (*bp != 0) {
	character = 0;
	if (*bp <= 0x7F) {
	    /* 1-byte codepoint */
	    character = *bp;
	} else if (*bp <= 0xBF) {
	    /* Invalid codepoint */
	    character = '?';
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
	if (character > 0x10FFFF) {
	    character = '?';
	}

	/* Convert the character to MS-JIS (or wait for another
	 * character I guess) */
	/* TODO: What about successor characters (e.g. variation selectors)? */
	hi = (character >> 16) & 0xFF;
	mid = (character >> 8) & 0xFF;
	lo = character & 0xFF;
	if (master_table[hi] == NULL) {
	    /* No MS-JIS encoding. */
	    codepoint = reverse_table0000['?'];
	} else {
	    subtable = master_table[hi];

	    if (subtable[mid] == NULL) {
		/* No MS-JIS encoding. */
		codepoint = reverse_table0000['?'];
	    } else {
		table = subtable[mid];
		if (table[lo].has_encoding == 0 &&
		    table[lo].successor_master_table == NULL) {
		    /* No MS-JIS encoding. */
		    codepoint = reverse_table0000['?'];
		} else {
		    codepoint = table[lo];
		}
	    }
	}

	if (codepoint.successor_master_table) {
	    /* Check the successive character. */
	    master_table = codepoint.successor_master_table;
	} else {
	    memcpy(output_ptr, codepoint.value, 2);
	    output_ptr += 2;
	    /* Reset to look at the master table again. */
	    master_table = reverse_master_table;
	}
    }
    if (master_table != reverse_master_table) {
	/* In this case, we must have read one character with no successor. */
	memcpy(output_ptr, codepoint.value, 2);
	output_ptr += 2;
    }
    /* Include the terminator */
    *output_ptr++ = '\0';
    *output_ptr = '\0';
    *size = (output_ptr - output) + 1;

    return output;
}  /* utf8_to_msjis */
