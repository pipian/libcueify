/* GenCue.c - A utility to generate CD cuesheets.
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "cdrom.h"
#include "toc.h"

int GenCuesheet(char *szFile, char cDriveLetter)
{
    /* Do some extra logging of disc data, since dBpoweramp falls down
     * on the job. */
    FILE *log = fopen(szFile, "wb");
    
    if (log != NULL) {
	time_t t = time(NULL);
	char szTime[256];
	HANDLE hDevice;
	CDROM_TOC toc;
	CDROM_TOC_SESSION_DATA session;
	unsigned char cdtextBacking[4096];
	CDROM_TOC_CD_TEXT_DATA *cdtext = (CDROM_TOC_CD_TEXT_DATA *)cdtextBacking;
	SUB_Q_CHANNEL_DATA data;
	int iTrack, iIndex;
	struct CDText *cdtextData;
	struct TrackIndices indices;
	
	hDevice = OpenVolume(cDriveLetter);
	if (hDevice != INVALID_HANDLE_VALUE) {
	    strftime(szTime, 256, "%Y-%m-%dT%H:%M:%S", gmtime(&t));
	    /* First read the last-session data. */
	    ReadLastSession(hDevice, &session);
	    fprintf(log,
		    "REM GENTIME \"%s\"\n"
		    "REM DRIVE \"%c\"\n",
		    szTime,
		    cDriveLetter);
	    fprintf(log,
		    "REM FIRSTSESSION %d\n"
		    "REM LASTSESSION %d\n"
		    "REM LASTSESSION TRACK %02d\n",
		    session.FirstCompleteSession,
		    session.LastCompleteSession,
		    session.TrackData[0].TrackNumber);
	    if (session.TrackData[0].Control != 0) {
		int iControl = session.TrackData[0].Control;
		
		fprintf(log, "REM LASTSESSION FLAGS");
		
		if (iControl & AUDIO_WITH_PREEMPHASIS) {
		    fprintf(log, " PRE");
		}
		if (iControl & DIGITAL_COPY_PERMITTED) {
		    fprintf(log, " DCP");
		}
		if (iControl & AUDIO_DATA_TRACK) {
		    fprintf(log, " DATA");
		}
		if (iControl & TWO_FOUR_CHANNEL_AUDIO) {
		    fprintf(log, " 4CH");
		}
		fprintf(log, "\n");
	    }
		    
	    fprintf(log,
		    "REM LASTSESSION INDEX 01 %02d:%02d:%02d\n",
		    session.TrackData[0].Address[1],
		    session.TrackData[0].Address[2],
		    session.TrackData[0].Address[3]);
	    
	    /* Then get the raw TOC data. */
	    ReadTOC(hDevice, &toc);
	    /* And the CD-Text. */
	    cdtext = ReadCDText(hDevice);
	    /* And actually parse the CD-Text. */
	    cdtextData = ParseCDText(cdtext, toc.LastTrack);
	    
	    /* Write out the MCN. */
	    if (ReadMCN(hDevice, &data) && data.MediaCatalog.Mcval) {
		char szMCN[16] = "";
		memcpy(szMCN, data.MediaCatalog.MediaCatalog, 15);
		
		fprintf(log, "CATALOG %s\n", szMCN);
	    }
	    
	    /* Print any disc-level CD-Text strings. */
	    if (cdtextData != NULL) {
		if (cdtextData->tracks[0].arranger != NULL) {
		    fprintf(log, "REM ARRANGER \"%s\"\n",
			    cdtextData->tracks[0].arranger);
		}
		if (cdtextData->tracks[0].composer != NULL) {
		    fprintf(log, "REM COMPOSER \"%s\"\n",
			    cdtextData->tracks[0].composer);
		}
		if (cdtextData->tracks[0].discId != NULL) {
		    fprintf(log, "REM DISK_ID \"%s\"\n",
			    cdtextData->tracks[0].discId);
		}
		if (cdtextData->tracks[0].genre != NULL) {
		    fprintf(log, "REM GENRE \"%s\"\n",
			    cdtextData->tracks[0].genre);
		}
		if (cdtextData->tracks[0].messages != NULL) {
		    fprintf(log, "REM MESSAGE \"%s\"\n",
			    cdtextData->tracks[0].messages);
		}
		if (cdtextData->tracks[0].performer != NULL) {
		    fprintf(log, "PERFORMER \"%s\"\n",
			    cdtextData->tracks[0].performer);
		}
		if (cdtextData->tracks[0].songwriter != NULL) {
		    fprintf(log, "SONGWRITER \"%s\"\n",
			    cdtextData->tracks[0].songwriter);
		}
		if (cdtextData->tracks[0].title != NULL) {
		    fprintf(log, "TITLE \"%s\"\n",
			    cdtextData->tracks[0].title);
		}
		if (cdtextData->tracks[0].tocInfo != NULL) {
		    fprintf(log, "REM TOC_INFO %s\n",
			    cdtextData->tracks[0].tocInfo);
		}
		if (cdtextData->tracks[0].tocInfo2 != NULL) {
		    fprintf(log, "REM TOC_INFO2 %s\n",
			    cdtextData->tracks[0].tocInfo2);
		}
		if (cdtextData->tracks[0].upc_ean != NULL) {
		    fprintf(log, "REM CATALOG %s\n",
			    cdtextData->tracks[0].upc_ean);
		}
		if (cdtextData->tracks[0].sizeInfo != NULL) {
		    fprintf(log, "REM SIZE_INFO %s\n",
			    cdtextData->tracks[0].sizeInfo);
		}
	    }
	    
	    /* And lastly the track stuff. */
	    for (iTrack = toc.FirstTrack; iTrack <= toc.LastTrack; iTrack++) {
		if (toc.TrackData[iTrack - 1].Control & AUDIO_DATA_TRACK) {
		    fprintf(log,
			    "FILE \"track%02d.bin\" BINARY\n"
			    "  TRACK %02d MODE1/2352\n",
			    iTrack, iTrack);
		} else {
		    fprintf(log,
			    "FILE \"track%02d.wav\" WAVE\n"
			    "  TRACK %02d AUDIO\n",
			    iTrack, iTrack);
		}
		
		if (cdtextData != NULL) {
		    if (cdtextData->tracks[iTrack].arranger != NULL) {
			fprintf(log, "    REM ARRANGER \"%s\"\n",
				cdtextData->tracks[iTrack].arranger);
		    }
		    if (cdtextData->tracks[iTrack].composer != NULL) {
			fprintf(log, "    REM COMPOSER \"%s\"\n",
				cdtextData->tracks[iTrack].composer);
		    }
		    if (cdtextData->tracks[iTrack].discId != NULL) {
			fprintf(log, "    REM DISK_ID \"%s\"\n",
				cdtextData->tracks[iTrack].discId);
		    }
		    if (cdtextData->tracks[iTrack].genre != NULL) {
			fprintf(log, "    REM GENRE \"%s\"\n",
				cdtextData->tracks[iTrack].genre);
		    }
		    if (cdtextData->tracks[iTrack].messages != NULL) {
			fprintf(log, "    REM MESSAGE \"%s\"\n",
				cdtextData->tracks[iTrack].messages);
		    }
		    if (cdtextData->tracks[iTrack].performer != NULL) {
			fprintf(log, "    PERFORMER \"%s\"\n",
				cdtextData->tracks[iTrack].performer);
		    }
		    if (cdtextData->tracks[iTrack].songwriter != NULL) {
			fprintf(log, "    SONGWRITER \"%s\"\n",
				cdtextData->tracks[iTrack].songwriter);
		    }
		    if (cdtextData->tracks[iTrack].title != NULL) {
			fprintf(log, "    TITLE \"%s\"\n",
				cdtextData->tracks[iTrack].title);
		    }
		    if (cdtextData->tracks[iTrack].tocInfo != NULL) {
			fprintf(log, "    REM TOC_INFO %s\n",
				cdtextData->tracks[iTrack].tocInfo);
		    }
		    if (cdtextData->tracks[iTrack].tocInfo2 != NULL) {
			fprintf(log, "    REM TOC_INFO2 %s\n",
				cdtextData->tracks[iTrack].tocInfo2);
		    }
		    if (cdtextData->tracks[iTrack].upc_ean != NULL) {
			fprintf(log, "    REM ISRC %s\n",
				cdtextData->tracks[iTrack].upc_ean);
		    }
		    if (cdtextData->tracks[iTrack].sizeInfo != NULL) {
			fprintf(log, "    REM SIZE_INFO %s\n",
				cdtextData->tracks[iTrack].sizeInfo);
		    }
		}
		
		if (ReadISRC(hDevice, iTrack, &data) && data.TrackIsrc.Tcval) {
		    char szISRC[16] = "";
		    memcpy(szISRC, data.TrackIsrc.TrackIsrc, 15);
		    
		    fprintf(log, "    ISRC %s\n", szISRC);
		}
		
		if (toc.TrackData[iTrack - 1].Control != 0) {
		    int iControl = toc.TrackData[iTrack - 1].Control;
		    
		    fprintf(log, "    FLAGS");
		    
		    if ((iControl & AUDIO_WITH_PREEMPHASIS) > 0) {
			fprintf(log, " PRE");
		    }
		    if ((iControl & DIGITAL_COPY_PERMITTED) > 0) {
			fprintf(log, " DCP");
		    }
		    if ((iControl & TWO_FOUR_CHANNEL_AUDIO) > 0) {
			fprintf(log, " 4CH");
		    }
		    fprintf(log, "\n");
		}
		
		fprintf(log,
			"    INDEX 01 %02d:%02d:%02d\n",
			toc.TrackData[iTrack - 1].Address[1],
			toc.TrackData[iTrack - 1].Address[2],
			toc.TrackData[iTrack - 1].Address[3]);
		
		/* Detect any other indices. */
		if (DetectTrackIndices(hDevice, &toc, iTrack, &indices)) {
		    for (iIndex = 1; iIndex < indices.iIndices; iIndex++) {
			fprintf(log,
				"    INDEX %02d %02d:%02d:%02d\n",
				iIndex + 1,
				indices.indices[iIndex].M,
				indices.indices[iIndex].S,
				indices.indices[iIndex].F);
		    }
		    free(indices.indices);
		}
	    }
	    
	    fprintf(log,
		    "REM LEADOUT %02d:%02d:%02d\n",
		    toc.TrackData[iTrack - 1].Address[1],
		    toc.TrackData[iTrack - 1].Address[2],
		    toc.TrackData[iTrack - 1].Address[3]);
	    
	    free(cdtext);
	    FreeCDText(cdtextData);
	    CloseVolume(hDevice);
	}
	
	fclose(log);
	
	return 0;
    }
    
    /* We failed?? */
    return 1;
}

int main(int argc, char *argv[])
{
    char *szDriveLetter;
    char *szCuesheet;
    
    /* Just got two command-line arguments we expect: drive letter and log. */
    if (argc != 3) {
	printf("Usage: GenCue DRIVELETTER CUESHEET\n");
	return 0;
    }
    
    szDriveLetter = argv[1];
    szCuesheet = argv[2];
    
    if (GenCuesheet(szCuesheet, szDriveLetter[0])) {
	printf("There was an issue writing the cuesheet!\n");
    }
    
    return 0;
}
