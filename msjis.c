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

char *ConvertMSJIS(char *szMSJIS, int iSize)
{
    int iOutputSize = 0, i;
    unsigned char cHi, cLo;
    const char * const *aszTable;
    const char *szChar;
    char *szOutput = NULL, *szOutputPtr;
    
    if (iSize < 0) {
	/* Count output size until we find a terminator/bad char. */
	szOutput = szMSJIS;
	do {
	    cHi = *szOutput++;
	    cLo = *szOutput++;
	    aszTable = masterTable[(unsigned char)cHi];
	    if (aszTable == NULL) {
		/* Don't know this table! */
		break;
	    } else if (aszTable[(unsigned char)cLo][0] != '\0') {
		iOutputSize += strlen(aszTable[(unsigned char)cLo]);
	    }
	} while (aszTable[(unsigned char)cLo] != '\0');
	/* And also include the terminator. */
	iOutputSize++;
    } else {
	/* Convert exactly iSize (wide) characters. */
	szOutput = szMSJIS;
	for (i = 0; i < iSize; i++) {
	    cHi = *szOutput++;
	    cLo = *szOutput++;
	    aszTable = masterTable[(unsigned char)cHi];
	    if (aszTable == NULL) {
		/* Don't know this table! */
		iOutputSize++;
	    } else if (aszTable[(unsigned char)cLo][0] == '\0') {
		iOutputSize++;
	    } else {
		iOutputSize += strlen(aszTable[(unsigned char)cLo]);
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
    szOutput[--iOutputSize] = '\0';
    
    /* Now do the conversion. */
    szOutputPtr = szOutput;
    while (iOutputSize > 0) {
	cHi = *szMSJIS++;
	cLo = *szMSJIS++;
	aszTable = masterTable[(unsigned char)cHi];
	if (aszTable == NULL) {
	    /* Don't know this table! */
	    *szOutputPtr++ = '\0';
	    iOutputSize--;
	} else if (aszTable[(unsigned char)cLo][0] == '\0') {
	    *szOutputPtr++ = '\0';
	    iOutputSize--;
	} else {
	    szChar = aszTable[(unsigned char)cLo];
	    strcpy(szOutputPtr, szChar);
	    szOutputPtr += strlen(szChar);
	    iOutputSize -= strlen(szChar);
	}
    }
    
    return szOutput;
}
