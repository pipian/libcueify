/* toc.h - Header for CD-ROM functions which read the TOC of a disc.
 *
 * Copyright (c) 2011 Ian Jacobi <pipian@pipian.com>
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

#ifndef _LIBCUEIFY_TOC_H
#define _LIBCUEIFY_TOC_H

/**
 * A transparent handle for the table of contents (TOC) of an audio CD.
 *
 * This is returned by cueify_device_read_toc() and is passed as the first
 * parameter to all cueify_toc_*() functions.
 */
typedef void *cueify_toc;


/**
 * This function reads the disc in the drive specified by the given
 * cueify_device object and returns a cueify_toc object for that disc.
 *
 * On error, this function returns NULL and sets an error message on
 * the cueify_device object which may be accessed using
 * cueify_device_get_error().
 *
 * The cueify_toc object returned by this function should be freed
 * using cueify_toc_free() when you are finished using it.
 *
 * @param d a cueify_device object representing the device from which
 *          the cueify_toc object should be read
 * @return a cueify_toc object, or NULL.
 */
cueify_toc *cueify_device_read_toc(cueify_device *device);


/**
 * Release the memory allocated for a cueify_toc object.
 *
 * @param t a cueify_toc object returned by cueify_device_read_toc()
 */
void cueify_toc_free(cueify_toc *t);


#define CUEIFY_LEAD_OUT_TRACK  0xAA  /** track number of the lead-out of a disc in the TOC */


/**
 * Return the number of the first track in the TOC.
 *
 * @param t a cueify_toc object
 * @return the number of the first track in the TOC
 */
uint8_t cueify_toc_get_first_track(cueify_toc *t);


/**
 * Return the number of the last track in the TOC.
 *
 * @param t a cueify_toc object
 * @return the number of the last track in the TOC
 */
uint8_t cueify_toc_get_last_track(cueify_toc *t);


#define CUEIFY_TOC_TRACK_HAS_PREEMPHASIS  0x1  /** the audio track has had preemphasis applied */
#define CUEIFY_TOC_TRACK_PERMITS_COPYING  0x2  /** the Digital Copy Permissions bit has been set on the audio track */
#define CUEIFY_TOC_TRACK_IS_DATA          0x4  /** the track is a data track */
#define CUEIFY_TOC_TRACK_IS_QUADRAPHONIC  0x8  /** the track contains quadraphonic (four channel) audio */


/**
 * Return the track control flags for a track in the TOC.
 *
 * trknum must be between the values of cueify_toc_get_first_track()
 * and cueify_toc_get_last_track(), inclusive or be
 * CUEIFY_LEAD_OUT_TRACK.  If trknum is outside these bounds, the
 * return value is undefined.
 *
 * @param t a cueify_toc object
 * @param trknum the number of the track for which flags should be returned
 * @return the track control flags for the track in the TOC
 */
uint8_t cueify_toc_get_track_control_flags(cueify_toc *t, uint8_t trknum);


#define CUEIFY_SUB_Q_NOTHING   0x00  /** sub-Q-channel contains nothing */
#define CUEIFY_SUB_Q_POSITION  0x01  /** sub-Q-channel contains the current position */
#define CUEIFY_SUB_Q_MCN       0x02  /** sub-Q-channel contains the media catalog number */
#define CUEIFY_SUB_Q_ISRC      0x03  /** sub-Q-channel contains an International Standard Recording Code (ISRC) */


/**
 * Return the format of the sub-Q-channel for a track in the TOC.
 *
 * trknum must be between the values of cueify_toc_get_first_track()
 * and cueify_toc_get_last_track(), inclusive, or be
 * CUEIFY_LEAD_OUT_TRACK.  If trknum is outside these bounds, the
 * return value is undefined.
 *
 * NOTE: In most cases, this function will return
 * CUEIFY_SUB_Q_NOTHING, however it is provided for completeness.
 *
 * @param t a cueify_toc object
 * @param trknum the number of the track for which the sub-Q-channel format should be returned
 * @return the contents of the sub-Q-channel for the track in the TOC
 */
uint8_t cueify_toc_get_sub_q_channel_contents(cueify_toc *t, uint8_t trknum);


/**
 * Return the starting offset of a track in the TOC.
 *
 * This offset is the number of frames from the start of the disc
 * which this track starts at (the LBA offset of the track)
 *
 * trknum must be between the values of cueify_toc_get_first_track()
 * and cueify_toc_get_last_track(), inclusive, or be
 * CUEIFY_LEAD_OUT_TRACK.  If trknum is outside these bounds, the
 * return value is undefined.
 *
 * @param t a cueify_toc object
 * @param trknum the number of the track for which the offset should be returned
 * @return the offset of the start of the track in the TOC
 */
uint32_t cueify_toc_get_track_offset(cueify_toc *t, uint8_t trknum);


/**
 * Return the total number of sectors in a TOC.
 *
 * NOTE: This macro is identical to
 * cueify_toc_get_track_offset(t, CUEIFY_LEAD_OUT_TRACK)
 *
 * @param t a cueify_toc object
 * @return the length of the disc represented by the toc
 */
#define cueify_toc_get_sectors(t)  \
  cueify_toc_get_track_offset(t, CUEIFY_LEAD_OUT_TRACK)


/**
 * Return the length of a track in the TOC.
 *
 * This length is the number of frames from the start of the track
 * to the start of the next track.
 *
 * trknum must be between the values of cueify_toc_get_first_track()
 * and cueify_toc_get_last_track(), inclusive.  If trknum is outside
 * these bounds, the return value is undefined.
 *
 * @param t a cueify_toc object
 * @param trknum the number of the track for which the offset should be returned
 * @return the length of the track in the TOC
 */
uint32_t cueify_toc_get_track_length(cueify_toc *t, uint8_t trknum);

#endif /* _LIBCUEIFY_TOC_H */
