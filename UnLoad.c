#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <winioctl.h>

typedef struct LoadArguments {
    TCHAR *szDrive;
    TCHAR *szPortName;
    /* TODO: Log. */
    TCHAR *szLogFile;
    /* TODO: PassErrorsBack. */
    TCHAR *szPassErrorsBack;
    DWORD dwWatchdog;
    DWORD dwElapsed;
    BOOL fRejectIfNoDisc;
    DWORD dwDrivesOnLeft;
} CmdArguments;

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

#define LOCK_RETRIES 3

BOOL LockVolume(HANDLE hVolume, DWORD dwWatchdog, DWORD *dwElapsed)
{
    DWORD dwBytesReturned;
    DWORD dwSleepAmount;
    int nTryCount;
    
    dwSleepAmount = dwWatchdog / LOCK_RETRIES;
    
    /* Do this in a loop until a timeout period has expired */
    for (nTryCount = 0; nTryCount < LOCK_RETRIES; nTryCount++) {
	if (DeviceIoControl(hVolume,
			    FSCTL_LOCK_VOLUME,
			    NULL, 0,
			    NULL, 0,
			    &dwBytesReturned,
			    NULL)) {
	    return TRUE;
	}
	
	Sleep(dwSleepAmount);
	*dwElapsed += dwSleepAmount;
    }
    
    return FALSE;
}

BOOL DismountVolume(HANDLE hVolume)
{
    DWORD dwBytesReturned;
    
    return DeviceIoControl(hVolume,
			   FSCTL_DISMOUNT_VOLUME,
			   NULL, 0,
			   NULL, 0,
			   &dwBytesReturned,
			   NULL);
}

BOOL PreventRemovalOfVolume(HANDLE hVolume, BOOL fPreventRemoval)
{
    DWORD dwBytesReturned;
    PREVENT_MEDIA_REMOVAL PMRBuffer;
    
    PMRBuffer.PreventMediaRemoval = fPreventRemoval;
    
    return DeviceIoControl(hVolume,
			   IOCTL_STORAGE_MEDIA_REMOVAL,
			   &PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL),
			   NULL, 0,
			   &dwBytesReturned,
			   NULL);
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
    BOOL fRemoveSafely = FALSE;
    BOOL fAutoEject = FALSE;
    
    /* Open the volume. */
    hVolume = OpenVolume(cDriveLetter);
    if (hVolume == INVALID_HANDLE_VALUE) {
	return FALSE;
    }
    
    /* Lock and dismount the volume. */
    if (LockVolume(hVolume, args->dwWatchdog, &args->dwElapsed) &&
	DismountVolume(hVolume)) {
	fRemoveSafely = TRUE;
	
	/* Set prevent removal to false and eject the volume. */
	if (PreventRemovalOfVolume(hVolume, FALSE) &&
	    AutoEjectVolume(hVolume)) {
	    fAutoEject = TRUE;
	}
    }
    
    /* Close the volume so other processes can use the drive. */
    if (!CloseVolume(hVolume)) {
	return FALSE;
    }
    
    return TRUE;
}

BOOL CloseCommPort(HANDLE hCommPort)
{
    return CloseHandle(hCommPort);
}

HANDLE OpenCommPort(TCHAR *szPortName)
{
    HANDLE hCommPort;
    DCB dcb = {0};
    
    dcb.DCBlength = sizeof(DCB);
    
    /* TODO: Spinlock? */
    hCommPort = CreateFile(szPortName,
			   GENERIC_READ | GENERIC_WRITE,
			   0,
			   NULL,
			   OPEN_EXISTING,
			   0,
			   NULL);
    
    if (hCommPort != INVALID_HANDLE_VALUE) {
	if (!GetCommState(hCommPort, &dcb)) {
	    CloseCommPort(hCommPort);
	    return INVALID_HANDLE_VALUE;
	}

	/* Set the CommState. */
	dcb.BaudRate = CBR_9600;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	
	if (!SetCommState(hCommPort, &dcb)) {
	    CloseCommPort(hCommPort);
	    return INVALID_HANDLE_VALUE;
	}
	
	/* And wait for characters received. */
	SetCommMask(hCommPort, EV_RXCHAR);
    }
    
    return hCommPort;
}

#define RIPPER_RESP_OK 'X'
#define RIPPER_RESP_ERROR 'E'
#define RIPPER_RESP_NONE '\0'

TCHAR SendRipperCommand(HANDLE hCommPort, TCHAR cCommand)
{
    DWORD dwBytesWritten;
    DWORD dwBytesRead;
    DWORD dwEventMask;
    TCHAR cResp;
    
    if (!WriteFile(hCommPort,
		   &cCommand,
		   1,
		   &dwBytesWritten,
		   NULL)) {
	return RIPPER_RESP_NONE;
    }
    
    /* Wait to receive the response character(s). */
    if (!WaitCommEvent(hCommPort,
		       &dwEventMask,
		       NULL)) {
	return RIPPER_RESP_NONE;
    }
    
    /* Check if the response is RIPPER_RESP_OK or RIPPER_RESP_ERROR. */
    if (!ReadFile(hCommPort,
		  &cResp,
		  1,
		  &dwBytesRead,
		  NULL)) {
	return RIPPER_RESP_NONE;
    }
    
    if (cResp == RIPPER_RESP_OK || cResp == RIPPER_RESP_ERROR) {
	return cResp;
    } else {
	return RIPPER_RESP_NONE;
    }
}

#define HAS_BIT_SET(dword, bit) (((dword >> bit) & 0x01) == 0x01)
#define RIPPER_CMD_ACCEPT_LEFT 'B'
#define RIPPER_CMD_ACCEPT_RIGHT 'A'

BOOL AcceptDisc(TCHAR cDriveLetter, CmdArguments *args)
{
    HANDLE hCommPort;
    TCHAR cAcceptDiscCommand = '\0';
    TCHAR cResp = '\0';
    DWORD dwBitOffset = 0;
    
    /* Which side? */
    if (cDriveLetter >= 'A' && cDriveLetter <= 'Z') {
	dwBitOffset = cDriveLetter - 'A';
    } else if (cDriveLetter >= 'a' && cDriveLetter <= 'z') {
	dwBitOffset = cDriveLetter - 'a';
    }
    
    if (HAS_BIT_SET(args->dwDrivesOnLeft, dwBitOffset)) {
	cAcceptDiscCommand = RIPPER_CMD_ACCEPT_LEFT;
    } else {
	cAcceptDiscCommand = RIPPER_CMD_ACCEPT_RIGHT;
    }
    
    /* Open the COM port. */
    hCommPort = OpenCommPort(args->szPortName);
    if (hCommPort == INVALID_HANDLE_VALUE) {
	return FALSE;
    }
    
    /* Send command to accept a disc from the given drive. */
    cResp = SendRipperCommand(hCommPort, cAcceptDiscCommand);
    
    /* Close the COM port so other processes can use the port. */
    if (!CloseCommPort(hCommPort)) {
	return FALSE;
    }
    
    return TRUE;
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
    if (AutoLoadVolume(hVolume)) {
	fAutoLoad = TRUE;
    }
    
    /* Close the volume so other processes can use the drive. */
    if (!CloseVolume(hVolume)) {
	return FALSE;
    }
    
    return TRUE;
}

void ParseArguments(int argc, char *argv[], CmdArguments *args)
{
    int nArgCount;
    char *szArg, *ptr;
    
    for (nArgCount = 0; nArgCount < argc - 1; nArgCount++) {
	szArg = argv[nArgCount + 1];
	
	/* Is it an argument we care about? */
	if (szArg[0] == '-' && szArg[1] == '-') {
	    switch (szArg[2]) {
	    case 'd':
		if (strncmp(szArg + 3, "rive=", 5) == 0) {
		    /* --drive */
		    args->szDrive = szArg + 8;
		    if (args->szDrive[0] == '"') {
			args->szDrive++;
			ptr = strchr(args->szDrive, '"');
			if (ptr != NULL) {
			    *ptr = '\0';
			}
		    }
		} else if (strncmp(szArg + 3, "rivesonleft=", 12) == 0) {
		    /* --drivesonleft */
		    ptr = szArg + 15;
		    if (*ptr == '"') {
			ptr++;
		    }
		    while (*ptr != '"' && *ptr != '\0') {
			if (*ptr >= 'a' && *ptr <= 'z') {
			    args->dwDrivesOnLeft |= 1 << (*ptr - 'a');
			} else if (*ptr >= 'A' && *ptr <= 'Z') {
			    args->dwDrivesOnLeft |= 1 << (*ptr - 'A');
			}
			ptr++;
		    }
		}
		break;
	    case 'l':
		/* --logfile */
		if (strncmp(szArg + 3, "ogfile=", 7) == 0) {
		    args->szLogFile = szArg + 10;
		    if (args->szLogFile[0] == '"') {
			args->szLogFile++;
			ptr = strchr(args->szLogFile, '"');
			if (ptr != NULL) {
			    *ptr = '\0';
			}
		    }
		}
		break;
	    case 'p':
		if (strncmp(szArg + 3, "asserrorsback=", 14) == 0) {
		    /* --passerrorsback */
		    args->szPassErrorsBack = szArg + 17;
		    if (args->szPassErrorsBack[0] == '"') {
			args->szPassErrorsBack++;
			ptr = strchr(args->szPassErrorsBack, '"');
			if (ptr != NULL) {
			    *ptr = '\0';
			}
		    }
		} else if (strncmp(szArg + 3, "ort=", 4) == 0) {
		    /* --port */
		    args->szPortName = szArg + 7;
		    if (args->szPortName[0] == '"') {
			args->szPortName++;
			ptr = strchr(args->szPortName, '"');
			if (ptr != NULL) {
			    *ptr = '\0';
			}
		    }
		}
		break;
	    case 'w':
		/* --watchdog */
		if (strncmp(szArg + 3, "atchdog=", 8) == 0) {
		    ptr = szArg + 11;
		    if (ptr[0] == '"') {
			ptr++;
			ptr = strchr(ptr, '"');
			if (ptr != NULL) {
			    *ptr = '\0';
			}
			ptr = szArg + 12;
		    }
		    args->dwWatchdog = atoi(ptr);
		}
		break;
	    case 'r':
		/* --rejectifnodisc */
		if (strcmp(szArg + 3, "ejectifnodisc") == 0) {
		    args->fRejectIfNoDisc = TRUE;
		}
		break;
	    default:
		/* Ignore this argument. */
		break;
	    }
	}
    }
}

int main(int argc, char *argv[])
{
    CmdArguments args;
    BOOL fContinue;
    
    args.szDrive = NULL;
    args.szPortName = NULL;
    args.szLogFile = NULL;
    args.szPassErrorsBack = NULL;
    args.dwWatchdog = 0;
    args.dwElapsed = 0;
    args.fRejectIfNoDisc = FALSE;
    args.dwDrivesOnLeft = 0;
    
    /* Parse command line arguments. */
    ParseArguments(argc, argv, &args);
    
    if (args.szDrive != NULL) {
	/* Eject the volume. */
	fContinue = EjectVolume(args.szDrive[0], &args);
	if (fContinue) {
	    /* Accept the disc with the robot. */
	    fContinue = AcceptDisc(args.szDrive[0], &args);
	}
	if (fContinue) {
	    /* Load the volume. */
	    fContinue = LoadVolume(args.szDrive[0], &args);
	}
    }
    
    return 0;
}
