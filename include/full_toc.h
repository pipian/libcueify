/* full_toc.h - Header for CD-ROM functions which read the full TOC
 * of a disc.
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

#ifndef _LIBCUEIFY_FULL_TOC_H
#define _LIBCUEIFY_FULL_TOC_H

/**
 * A transparent handle for the full table of contents (TOC) of an audio CD.
 *
 * Full table of contents data subsumes basic table of contents data,
 * but may be available on fewer operating systems.
 *
 * This is returned by cueify_full_toc_new() and is passed as the first
 * parameter to all cueify_full_toc_*() functions.
 */
typedef void *cueify_full_toc;


/**
 * Create a new full TOC instance. The instance is created with no
 * tracks, and should be populated using
 * cueify_device_read_full_toc() or cueify_full_toc_deserialize().
 *
 * @return NULL if there was an error allocating memory, else the new full TOC
 */
cueify_full_toc *cueify_full_toc_new();


/**
 * Read the full TOC of the disc in the optical disc device associated
 * with a device handle.
 *
 * @pre { d != NULL, t != NULL }
 * @param d an opened device handle
 * @param t a full TOC instance to populate
 * @return CUEIFY_OK if the full TOC was successfully read; otherwise
 *         an error code is returned
 */
int cueify_device_read_full_toc(cueify_device *d, cueify_full_toc *t);


/**
 * Deserialize a full TOC instance previously serialized with
 * cueify_full_toc_serialize().
 *
 * @note This serialization is, in principle, the same as that
 *       returned by the MMC READ TOC/PMA/ATIP command with format
 *       0010b, excluding any track descriptors with ADR=5.
 *
 * @pre { t != NULL, buffer != NULL }
 * @param t a full TOC instance to populate
 * @param buffer a pointer to the serialized full TOC data
 * @param size the size of the buffer
 * @return CUEIFY_OK if the full TOC was successfully deserialized;
 *         otherwise an error code is returned
 */
int cueify_full_toc_deserialize(cueify_full_toc *t, uint8_t *buffer, size_t size);


/**
 * Serialize a full TOC instance for later deserialization with
 * cueify_full_toc_deserialize().
 * @note This serialization is, in principle, the same as that
 *       returned by the MMC READ TOC/PMA/ATIP command with format
 *       0010b, excluding any track descriptors with ADR=5.
 *
 * @pre { t != NULL, size != NULL }
 * @param t a full TOC instance to serialize
 * @param buffer a pointer to a location to serialize data to, or NULL
 *               to determine the optimal size of such a buffer
 * @param size a pointer to the size of the buffer. When called, the
 *             size must contain the maximum number of bytes that may
 *             be stored in buffer. When this function is complete,
 *             the pointer will contain the actual number of bytes
 *             serialized, or if buffer is NULL, the number of bytes
 *             needed to fully serialize the full TOC instance.
 * @return CUEIFY_OK if the full TOC was successfully serialized; otherwise
 *         an error code is returned
 */
int cueify_full_toc_serialize(cueify_full_toc *t, uint8_t *buffer, size_t *size);


/**
 * Free a full TOC instance. Deletes the object pointed to by t.
 *
 * @pre { t != NULL }
 * @param t a cueify_full_toc object created by cueify_full_toc_new()
 */
void cueify_full_toc_free(cueify_full_toc *t);


/**
 * Get the number of the first session in a full TOC instance.
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the first session in t
 */
uint8_t cueify_full_toc_get_first_session(cueify_full_toc *t);


/**
 * Get the number of the last session in a full TOC instance.
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the last session in t
 */
uint8_t cueify_full_toc_get_last_session(cueify_full_toc *t);


/**
 * Get the number of the session for a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)
 *        OR track == CUEIFY_LEAD_OUT_TRACK OR 0xA0 <= track <= 0xA2 }
 * @param t a full TOC instance
 * @param track the number of the track for which the session number should be returned
 * @return the number of the last session in t
 */
uint8_t cueify_full_toc_get_track_session(cueify_full_toc *t, uint8_t track);


/**
 * Get the track control flags for a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)
 *        OR track == CUEIFY_LEAD_OUT_TRACK OR 0xA0 <= track <= 0xA2 }
 * @param t a full TOC instance
 * @param track the number of the track for which control flags should be returned
 * @return the control flags for track number track in t
 */
uint8_t cueify_full_toc_get_track_control_flags(cueify_full_toc *t, uint8_t track);


/**
 * Get the format of the sub-Q-channel for a track in a full TOC instance.
 *
 * @note In most cases, this function will return
 *       CUEIFY_SUB_Q_NOTHING, however it is provided for completeness.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)
 *        OR track == CUEIFY_LEAD_OUT_TRACK OR 0xA0 <= track <= 0xA2 }
 * @param t a full TOC instance
 * @param track the number of the track for which the sub-Q-channel
 *              format should be returned
 * @return the contents of the sub-Q-channel for track number track in t
 */
uint8_t cueify_full_toc_get_track_sub_q_channel_contents(cueify_full_toc *t,
							 uint8_t track);


/**
 * Get the absolute time of a point in the lead-in in a full TOC
 * instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)
 *        OR track == CUEIFY_LEAD_OUT_TRACK OR 0xA0 <= track <= 0xA2}
 * @param t a full TOC instance
 * @param point the point in the lead-in for which the absolute time
 *              should be returned
 * @return the absolute time of point in t
 */
cueify_msf_t cueify_full_toc_get_point_address(cueify_full_toc *t, uint8_t point);


/**
 * Get the time of the start address of a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)
 *        OR track == CUEIFY_LEAD_OUT_TRACK }
 * @param t a full TOC instance
 * @param track the number of the track for which the time of the
 *        start address should be returned
 * @return the time of the start address of track number track in t
 */
cueify_msf_t cueify_full_toc_get_track_address(cueify_full_toc *t, uint8_t track);


/**
 * Get the number of the first track in a full TOC instance.
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the first track in t
 */
uint8_t cueify_full_toc_get_first_track(cueify_full_toc *t);


/**
 * Get the number of the last track in a full TOC instance.
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the last track in t
 */
uint8_t cueify_full_toc_get_last_track(cueify_full_toc *t);


#define CUEIFY_DISC_MODE_1  0x00  /** The disc is a CD-DA disc, or CD Data with the first track in Mode 1. */
#define CUEIFY_DISC_CDI     0x01  /** The disc is a CD-I disc. */
#define CUEIFY_DISC_MODE_2  0x02  /** The disc is CD-XA disc with the first track in Mode 2. */


/**
 * Get the disc type in a full TOC instance.
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the disc type of t
 */
uint8_t cueify_full_toc_get_disc_type(cueify_full_toc *t);


/**
 * Get the total length of a full TOC instance.
 *
 * @note Shorthand for cueify_full_toc_get_start_address(t, CUEIFY_LEAD_OUT_TRACK).
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the total length of t
 */
#define cueify_full_toc_get_disc_length(t)  \
    cueify_full_toc_get_track_address(t, CUEIFY_LEAD_OUT_TRACK)


/**
 * Get the total length of a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track <= cueify_full_toc_get_last_track(t)}
 * @param t a full TOC instance
 * @param track the number of the track for which the length should be returned
 * @return the length of track number track in t
 */
cueify_msf_t cueify_full_toc_get_track_length(cueify_full_toc *t, uint8_t track);

#endif /* _LIBCUEIFY_FULL_TOC_H */
