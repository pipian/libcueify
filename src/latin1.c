/* latin1.c - (CD-Text) ISO 8859-1 to UTF-8 text codec functions.
 *
 * Copyright (c) 2011, 2012 Ian Jacobi <pipian@pipian.com>
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
#include "latin1_tables.h"

char *latin1_to_utf8(uint8_t *latin1, int size)
{
    int output_size = 0, i;
    const char *character;
    char *output = NULL, *output_ptr;

    if (size < 0) {
	/* Count output size until we find a terminator/bad char. */
	output = (char *)latin1;
	while (table[(unsigned char)*output] != '\0') {
	    output_size += strlen(table[(unsigned char)*output]);
	}
	/* And also include the terminator. */
	output_size++;
    } else {
	/* Convert exactly size characters. */
	output = (char *)latin1;
	for (i = 0; i < size; i++) {
	  if (table[(unsigned char)latin1[i]][0] == '\0') {
		output_size++;
	    } else {
		output_size += strlen(table[(unsigned char)latin1[i]]);
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
    /* Already set the terminator. */
    output[--output_size] = '\0';

    /* Now do the conversion. */
    output_ptr = output;
    while (output_size > 0) {
	character = table[(unsigned char)*latin1++];
	strcpy(output_ptr, character);
	if (*character == '\0') {
	    output_ptr++;
	    output_size--;
	} else {
	    output_ptr += strlen(character);
	    output_size -= strlen(character);
	}
    }

    return output;
}  /* latin1_to_utf8 */


size_t latin1_byte_count(char *utf8) {
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
	if (character > 0x10FFFF) {
	    character = '?';
	}

	/* Convert the character to ISO 8859-1 (or wait for another
	 * character I guess) */
	hi = (character >> 16) & 0xFF;
	mid = (character >> 8) & 0xFF;
	lo = character & 0xFF;
    retry_encoding:
	if (master_table[hi] == NULL) {
	    /* No ISO 8859-1 encoding. */
	    if (codepoint.successor_master_table != NULL) {
		/* We must have been trying to find a successor. */
		size++;
		/* Reset to look at the master table and try again. */
		codepoint.successor_master_table = NULL;
		master_table = reverse_master_table;
		goto retry_encoding;
	    } else {
		codepoint = reverse_table0000['?'];
	    }
	} else {
	    subtable = master_table[hi];

	    if (subtable[mid] == NULL) {
		/* No ISO 8859-1 encoding. */
		if (codepoint.successor_master_table != NULL) {
		    /* We must have been trying to find a successor. */
		    size++;
		    /* Reset to look at the master table and try again. */
		    codepoint.successor_master_table = NULL;
		    master_table = reverse_master_table;
		    goto retry_encoding;
		} else {
		    codepoint = reverse_table0000['?'];
		}
	    } else {
		table = subtable[mid];
		if (table[lo].has_encoding == 0 &&
		    table[lo].successor_master_table == NULL) {
		    /* No ISO 8859-1 encoding. */
		    if (codepoint.successor_master_table != NULL) {
			/* We must have been trying to find a successor. */
			size++;
			/* Reset to look at the master table and try again. */
			codepoint.successor_master_table = NULL;
			master_table = reverse_master_table;
			goto retry_encoding;
		    } else {
			codepoint = reverse_table0000['?'];
		    }
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

    return size;  /* 1-byte character set */
}  /* latin1_byte_count */


uint8_t *utf8_to_latin1(char *utf8, size_t *size) {
    uint8_t *output = NULL, *output_ptr = NULL;
    uint8_t *bp = (uint8_t *)utf8;
    uint8_t hi, mid, lo;
    uint32_t character;
    struct multibyte_codepoint codepoint;
    const struct multibyte_codepoint * const * const *master_table;
    const struct multibyte_codepoint * const *subtable;
    const struct multibyte_codepoint *table;

    *size = (strlen(utf8) + 1);
    output = malloc(*size);
    if (output == NULL) {
	return NULL;
    }
    output_ptr = output;

    master_table = reverse_master_table;
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
	if (character > 0x10FFFF) {
	    character = '?';
	}

	/* Convert the character to ISO 8859-1 (or wait for another
	 * character I guess) */
	hi = (character >> 16) & 0xFF;
	mid = (character >> 8) & 0xFF;
	lo = character & 0xFF;
    retry_encoding:
	if (master_table[hi] == NULL) {
	    /* No ISO 8859-1 encoding. */
	    if (codepoint.successor_master_table != NULL) {
		/* We must have been trying to find a successor. */
		*output_ptr++ = codepoint.value[1];
		/* Reset to look at the master table and try again. */
		codepoint.successor_master_table = NULL;
		master_table = reverse_master_table;
		goto retry_encoding;
	    } else {
		codepoint = reverse_table0000['?'];
	    }
	} else {
	    subtable = master_table[hi];

	    if (subtable[mid] == NULL) {
		/* No ISO 8859-1 encoding. */
		if (codepoint.successor_master_table != NULL) {
		    /* We must have been trying to find a successor. */
		    *output_ptr++ = codepoint.value[1];
		    /* Reset to look at the master table and try again. */
		    codepoint.successor_master_table = NULL;
		    master_table = reverse_master_table;
		    goto retry_encoding;
		} else {
		    codepoint = reverse_table0000['?'];
		}
	    } else {
		table = subtable[mid];
		if (table[lo].has_encoding == 0 &&
		    table[lo].successor_master_table == NULL) {
		    /* No ISO 8859-1 encoding. */
		    if (codepoint.successor_master_table != NULL) {
			/* We must have been trying to find a successor. */
			*output_ptr++ = codepoint.value[1];
			/* Reset to look at the master table and try again. */
			codepoint.successor_master_table = NULL;
			master_table = reverse_master_table;
			goto retry_encoding;
		    } else {
			codepoint = reverse_table0000['?'];
		    }
		} else {
		    codepoint = table[lo];
		}
	    }
	}

	if (codepoint.successor_master_table) {
	    /* Check the successive character. */
	    master_table = codepoint.successor_master_table;
	} else {
	    *output_ptr++ = codepoint.value[1];
	    /* Reset to look at the master table again. */
	    master_table = reverse_master_table;
	}
    }
    if (master_table != reverse_master_table) {
	/* In this case, we must have read one character with no successor. */
	*output_ptr++ = codepoint.value[1];
    }
    /* Include the terminator */
    *output_ptr = '\0';
    *size = (output_ptr - output) + 1;

    return output;
}  /* utf8_to_latin1 */
