/* cdrom.c - CD-ROM handling functions for the MFDigital 7602/7604 Ripstation
 *           driver for dBpoweramp BatchRipper.
 *
 * Copyright (c) 2010 Ian Jacobi
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
#include <tchar.h>
#include <winioctl.h>
#include "7604args.h"
#include "cdrom.h"

LPTSTR szVolumeFormat = TEXT("\\\\.\\%c:");
LPTSTR szRootFormat = TEXT("%c:\\");

HANDLE OpenVolume(TCHAR cDriveLetter)
{
    HANDLE hVolume;
    UINT uDriveType;
    TCHAR szVolumeName[8];
    TCHAR szRootName[5];
    DWORD dwAccessFlags;
    
    wsprintf(szRootName, szRootFormat, cDriveLetter);
    
    uDriveType = GetDriveType(szRootName);
    switch (uDriveType) {
    case DRIVE_REMOVABLE:
	dwAccessFlags = GENERIC_READ | GENERIC_WRITE;
	break;
    case DRIVE_CDROM:
	dwAccessFlags = GENERIC_READ;
	break;
    default:
	return INVALID_HANDLE_VALUE;
    }
    
    wsprintf(szVolumeName, szVolumeFormat, cDriveLetter);
    
    hVolume = CreateFile(szVolumeName,
			 dwAccessFlags,
			 FILE_SHARE_READ | FILE_SHARE_WRITE,
			 NULL,
			 OPEN_EXISTING,
			 0,
			 NULL);
    return hVolume;
}

BOOL CloseVolume(HANDLE hVolume)
{
    return CloseHandle(hVolume);
}

BOOL AutoEjectVolume(HANDLE hVolume)
{
    DWORD dwBytesReturned;
    
    return DeviceIoControl(hVolume,
			   IOCTL_STORAGE_EJECT_MEDIA,
			   NULL, 0,
			   NULL, 0,
			   &dwBytesReturned,
			   NULL);
}

BOOL EjectVolume(TCHAR cDriveLetter, CmdArguments *args)
{
    HANDLE hVolume;
    BOOL fAutoEject = FALSE;
    
    /* Open the volume. */
    hVolume = OpenVolume(cDriveLetter);
    if (hVolume == INVALID_HANDLE_VALUE) {
	return FALSE;
    }
    
    /* Eject the volume without locking (since dBpoweramp already locks). */
    if (!AutoEjectVolume(hVolume)) {
	fAutoEject = TRUE;
    }
    
    /* Close the volume so other processes can use the drive. */
    if (!CloseVolume(hVolume)) {
	return FALSE;
    }
    
    return fAutoEject;
}

BOOL AutoLoadVolume(HANDLE hVolume)
{
    DWORD dwBytesReturned;
    
    return DeviceIoControl(hVolume,
			   IOCTL_STORAGE_LOAD_MEDIA,
			   NULL, 0,
			   NULL, 0,
			   &dwBytesReturned,
			   NULL);
}

BOOL LoadVolume(TCHAR cDriveLetter, CmdArguments *args)
{
    HANDLE hVolume;
    BOOL fAutoLoad = FALSE;
    
    /* Open the volume. */
    hVolume = OpenVolume(cDriveLetter);
    if (hVolume == INVALID_HANDLE_VALUE) {
	return FALSE;
    }
    
    /* Load the volume. */
    if (!AutoLoadVolume(hVolume)) {
	fAutoLoad = TRUE;
    }
    
    /* Close the volume so other processes can use the drive. */
    if (!CloseVolume(hVolume)) {
	return FALSE;
    }
    
    return fAutoLoad;
}

BOOL CheckVolume(HANDLE hVolume)
{
    DWORD dwBytesReturned;
    
    return DeviceIoControl(hVolume,
			   IOCTL_STORAGE_CHECK_VERIFY,
			   NULL, 0,
			   NULL, 0,
			   &dwBytesReturned,
			   NULL);
}

BOOL VolumeHasMedia(TCHAR cDriveLetter, CmdArguments *args)
{
    HANDLE hVolume;
    BOOL fHasMedia = FALSE;
    
    /* Open the volume. */
    hVolume = OpenVolume(cDriveLetter);
    if (hVolume == INVALID_HANDLE_VALUE) {
	return FALSE;
    }
    
    /* Test the volume for media. */
    if (CheckVolume(hVolume)) {
	fHasMedia = TRUE;
    }
    
    /* Close the volume so other processes can use the drive. */
    CloseVolume(hVolume);
    
    return fHasMedia;
}
