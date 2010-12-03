/* cdrom.h - CD-ROM handling functions for the MFDigital 7602/7604 Ripstation
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

#ifndef _7604DRIVERS_CDROM_H
#define _7604DRIVERS_CDROM_H

#include <windows.h>
#include <tchar.h>
#include "7604args.h"

/** Open the drive given by the drive letter in cDriveLetter.
 *
 * @param cDriveLetter The letter of the drive to open.
 * @return An opened drive handle of the given drive, or INVALID_HANDLE_VALUE.
 */
HANDLE OpenVolume(TCHAR cDriveLetter);

/** Close the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to the volume to close.
 * @return TRUE if the handle was closed successfully.
 */
BOOL CloseVolume(HANDLE hVolume);

/** Lock the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to the volume to lock.
 * @param dwWatchdog The amount of time before the command should fail.
 * @param dwElapsed The amount of time elapsed when the volume was locked (OUT)
 * @return TRUE if the volume was locked.
 */
BOOL LockVolume(HANDLE hVolume, DWORD dwWatchdog, DWORD *dwElapsed);

/** Dismount the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to the volume to dismount.
 * @return TRUE if the volume dismounted.
 */
BOOL DismountVolume(HANDLE hVolume);

/** Set whether the volume should be prevented from being removed.
 *
 * @param hVolume A handle to the volume to set the removal prevention for.
 * @param fPreventRemoval Whether or not to prevent removal of the volume.
 * @return TRUE if the volume was prevented from being removed.
 */
BOOL PreventRemovalOfVolume(HANDLE hVolume, BOOL fPreventRemoval);

/** Physically eject the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to the volume to eject.
 * @return TRUE if the volume ejected.
 */
BOOL AutoEjectVolume(HANDLE hVolume);

/** Eject the volume with the given drive letter.
 *
 * @param cDriveLetter The letter of the drive to eject.
 * @param args Any command line arguments to the executable.
 * @return TRUE if the volume successfully ejected.
 */
BOOL EjectVolume(TCHAR cDriveLetter, CmdArguments *args);

/** Physically load the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to a volume to load.
 * @return TRUE if the volume successfully loaded.
 */
BOOL AutoLoadVolume(HANDLE hVolume);

/** Load the volume specified by cDriveLetter.
 *
 * @param cDriveLetter The letter of the drive to load a disc into.
 * @param args The command-line arguments (e.g. --drivesonleft) of the
 *             executable.
 * @return TRUE if the disc was loaded successfully.
 */
BOOL LoadVolume(TCHAR cDriveLetter, CmdArguments *args);

/** Check the volume identified by the handle hVolume.
 *
 * @param hVolume A handle to a volume to check.
 * @return TRUE if the volume contains media.
 */
BOOL CheckVolume(HANDLE hVolume);

/** Determine if the volume specified by cDriveLetter has media in it.
 *
 * @param cDriveLetter The letter of the drive to test.
 * @param args The command-line arguments (e.g. --drivesonleft) of the
 *             executable.
 * @return TRUE if the drive specified by cDriveLetter contains media.
 */
BOOL VolumeHasMedia(TCHAR cDriveLetter, CmdArguments *args);

#endif
