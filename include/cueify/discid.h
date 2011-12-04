/* discid.h - Header for CD-ROM discid calculation functions.
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

#ifndef _CUEIFY_DISCID_H
#define _CUEIFY_DISCID_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * Calculate the freedb discid from the provided TOC.
 *
 * @pre { t has been initialized }
 * @param t the TOC of the disc for which the freedb discid should be
 *          calculated
 * @param s the multisession data of the disc for which the freedb
 *          discid should be calculated.  If not NULL, the discid will
 *          be calculated like MusicBrainz, by ignoring the data
 *          tracks at the end of the disc as hinted by the
 *          multisession data.  Otherwise, data tracks will be
 *          included.
 * @return the freedb discid as an unsigned integer
 */
uint32_t cueify_toc_get_freedb_id(cueify_toc *t, cueify_sessions *s);


/**
 * Calculate the freedb discid from the provided full TOC.  libcueify
 * will only use audio tracks to calculate the discid.
 *
 * @pre { t has been initialized }
 * @param t the full TOC for which the freedb discid should be calculated
 * @param use_data_tracks if 1, the freedb discid will be calculated using 
 *                        data tracks.  If 0, the freedb discid will be
 *                        calculated without data tracks (e.g. like the
 *                        MusicBrainz discid)
 * @return the freedb discid as an unsigned integer
 */
uint32_t cueify_full_toc_get_freedb_id(cueify_full_toc *t,
				       int use_data_tracks);


/**
 * Calculate the freedb discid from the disc currently in an optical
 * disc (CD-ROM) device.
 *
 * @pre { d != NULL }
 * @param d the device containing the disc for which the freedb discid
 *          should be calculated
 * @param use_data_tracks if 1, the freedb discid will be calculated using 
 *                        data tracks.  If 0, the freedb discid will be
 *                        calculated without data tracks (e.g. like the
 *                        MusicBrainz discid)
 * @return the freedb discid as an unsigned integer, or 0 if there was
 *         an error reading from the device
 */
uint32_t cueify_device_get_freedb_id(cueify_device *d, int use_data_tracks);


/**
 * Calculate the MusicBrainz discid from the provided TOC and
 * multisession data.
 *
 * @pre { t has been initialized }
 * @param t the TOC of the disc for which the MusicBrainz discid
 *          should be calculated
 * @param s the multisession data of the disc for which the
 *          MusicBrainz discid should be calculated.  If NULL,
 *          heuristics will be applied to guess whether or not the
 *          disc has multiple sessions.
 * @return a null-terminated string containing the MusicBrainz discid.
 *         The string must be freed.
 */
char *cueify_toc_get_musicbrainz_id(cueify_toc *t, cueify_sessions *s);


/**
 * Calculate the MusicBrainz discid from the provided full TOC.
 *
 * @pre { t has been initialized }
 * @param t the full TOC of the disc for which the MusicBrainz discid
 *          should be calculated
 * @return a null-terminated string containing the MusicBrainz discid.
 *         The string must be freed.
 */
char *cueify_full_toc_get_musicbrainz_id(cueify_full_toc *t);


/**
 * Calculate the MusicBrainz discid from the disc currently in an optical
 * disc (CD-ROM) device.
 *
 * @pre { d != NULL }
 * @param d the device containing the disc for which the MusicBrainz
 *          discid should be calculated
 * @return a null-terminated string containing the MusicBrainz discid.
 *         The string must be freed.
 */
char *cueify_device_get_musicbrainz_id(cueify_device *d);

#ifdef __cplusplus
};  /* extern "C" */
#endif  /* __cplusplus */

#endif /* _CUEIFY_DISCID_H */
