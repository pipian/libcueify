/* 7604args.h - Header for command-line argument parsing functions for the
 *              MFDigital 7602/7604 Ripstation driver for dBpoweramp
 *              BatchRipper.
 *
 * Copyright (c) 2010,2011 Ian Jacobi
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

#ifndef _7604DRIVERS_7604ARGS_H
#define _7604DRIVERS_7604ARGS_H

#include <windows.h>

/** A struct to hold command-line arguments. */
typedef struct LoadArguments {
    /* The drive letter to work with. */
    TCHAR *szDrive;
    /* The port name to control the arm with. */
    TCHAR *szPortName;
    /* The log file to write. (Unsupported) */
    TCHAR *szLogFile;
    /* The error file to pass errors back with. */
    TCHAR *szPassErrorsBack;
    /* The amount of time (in milliseconds) before the process is killed. */
    DWORD dwWatchdog;
    /* The amount of time elapsed (in milliseconds) roughly. */
    DWORD dwElapsed;
    /* Whether or not to reject if the disc can't be read (Unsupported) */
    BOOL fRejectIfNoDisc;
    /* Bitmask where true bits mark drives on the left (LSB A:\, etc.) */
    DWORD dwDrivesOnLeft;
    /* Whether or not to shake the arm on loading a disc from the stack. */
    BOOL fShake;
    /* The directory to write cuesheet files to. */
    TCHAR *szCuesheetDirectory;
} CmdArguments;

/** Parse command-line arguments.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of character strings being the command-line arguments.
 * @param args A pointer to a CmdArguments structure to store parsed arguments.
 */
void ParseArguments(int argc, char *argv[], CmdArguments *args);

/** Determines whether the given drive is on the left.
 *
 * @param dwDrivesOnLeft Bitmask where true bits mark drives on the left (LSB A:\, etc.)
 * @param cDriveLetter The letter of the drive to determine.
 * @return TRUE if the drive in cDriveLetter is on the left.
 */
BOOL IsDriveOnLeft(DWORD dwDrivesOnLeft, char cDriveLetter);

/** Write a string to the log file, if specified.
 *
 * @param args The command line arguments of the app.
 * @param string A null-terminated pointer to a character string to write to the log file.
 */
void Log(CmdArguments *args, char *s);

#endif
