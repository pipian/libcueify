/* latin1.c - (CD-Text) ISO 8859-1 to UTF-8 text codec functions.
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
#include "latin1_tables.h"

char *ConvertLatin1(char *szLatin1, int iSize)
{
    int iOutputSize = 0, i;
    const char *szChar;
    char *szOutput = NULL, *szOutputPtr;
    
    if (iSize < 0) {
	/* Count output size until we find a terminator/bad char. */
	szOutput = szLatin1;
	while (table[(unsigned char)*szOutput] != '\0') {
	    iOutputSize += strlen(table[(unsigned char)*szOutput]);
	}
	/* And also include the terminator. */
	iOutputSize++;
    } else {
	/* Convert exactly iSize characters. */
	szOutput = szLatin1;
	for (i = 0; i < iSize; i++) {
	  if (table[(unsigned char)szLatin1[i]][0] == '\0') {
		iOutputSize++;
	    } else {
		iOutputSize += strlen(table[(unsigned char)szLatin1[i]]);
	    }
	}
	/* And also add a terminator. */
	iOutputSize++;
    }
    
    /* Allocate space for the conversion... */
    szOutput = calloc(iOutputSize, 1);
    if (szOutput == NULL) {
	return NULL;
    }
    /* Already set the terminator. */
    szOutput[--iOutputSize] = '\0';
    
    /* Now do the conversion. */
    szOutputPtr = szOutput;
    while (iOutputSize > 0) {
	szChar = table[(unsigned char)*szLatin1++];
	strcpy(szOutputPtr, szChar);
	if (*szChar == '\0') {
	    szOutputPtr++;
	    iOutputSize--;
	} else {
	    szOutputPtr += strlen(szChar);
	    iOutputSize -= strlen(szChar);
	}
    }
    
    return szOutput;
}
