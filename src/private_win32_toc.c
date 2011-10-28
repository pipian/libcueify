/* toc.c - Advanced TOC-based CD-ROM functions.
 *
 * Copyright (c) 2010, 2011 Ian Jacobi
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

#include <windows.h>
#include "charsets.h"
#include "toc.h"

/** Convert an MSF address into the corresponding LBA address.
 *
 * @param address An MSF address.
 * @return The corresponding LBA address.
 */
static int AddressToSectors(UCHAR address[4])
{
    return (((address[1] * 60) + address[2]) * 75) + address[3];
}

/** Convert an LBA address into the corresponding MSF address.
 *
 * @param sectors An LBA address.
 * @param m A pointer to store the minute value of the MSF address.
 * @param s A pointer to store the second value of the MSF address.
 * @param f A pointer to store the frame value of the MSF address.
 */
static void SectorsToAddress(int sectors, int *m, int *s, int *f)
{
    *m = sectors / (60 * 75);
    *s = sectors / 75 % 60;
    *f = sectors % 75;
}

unsigned char *ReadRawSector(HANDLE hDevice, unsigned int lba, UCHAR trackMode)
{
    DWORD dwReturned;
    RAW_READ_INFO readInfo;
    unsigned char *sector = NULL;
    
    sector = malloc(RAW_SECTOR_SIZE);
    if (sector == NULL) {
	return NULL;
    }
    
    readInfo.DiskOffset.QuadPart = lba * 2048;
    readInfo.SectorCount = 1;
    switch (trackMode) {
    case 0x00: /* CD */
	readInfo.TrackMode = YellowMode2;
	break;
    case 0x10: /* CD-I */
    case 0x20: /* CD-XA */
	readInfo.TrackMode = XAForm2;
	break;
    default:
	readInfo.TrackMode = YellowMode2;
	break;
    }

    if (!DeviceIoControl(hDevice,
			 IOCTL_CDROM_RAW_READ,
			 &readInfo, sizeof(readInfo),
			 sector, RAW_SECTOR_SIZE,
			 &dwReturned, NULL)) {
	free(sector);
	sector = NULL;
    }
    
    return sector;
}
