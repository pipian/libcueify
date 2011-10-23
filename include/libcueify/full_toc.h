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

#include <libcueify/device.h>
#include <libcueify/constants.h>
#include <libcueify/types.h>

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
int cueify_full_toc_deserialize(cueify_full_toc *t, uint8_t *buffer,
				size_t size);


/**
 * Serialize a full TOC instance for later deserialization with
 * cueify_full_toc_deserialize().
 *
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
int cueify_full_toc_serialize(cueify_full_toc *t, uint8_t *buffer,
			      size_t *size);


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
 *        cueify_full_toc_get_first_track(t) <= track AND
 *        track <= cueify_full_toc_get_last_track(t) }
 * @param t a full TOC instance
 * @param track the number of the track for which the session number
 *              should be returned
 * @return the number of the last session in t
 */
uint8_t cueify_full_toc_get_track_session(cueify_full_toc *t, uint8_t track);


/**
 * Get the track control flags for a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track AND
 *        track <= cueify_full_toc_get_last_track(t) }
 * @param t a full TOC instance
 * @param track the number of the track for which control flags should
 *              be returned
 * @return the control flags for track number track in t
 */
uint8_t cueify_full_toc_get_track_control_flags(cueify_full_toc *t,
						uint8_t track);


/**
 * Get the format of the sub-Q-channel of the block in which a track
 * in a full TOC instance was found.
 *
 * @note In most cases, this function will return
 *       CUEIFY_SUB_Q_POSITION, however it is provided for completeness.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track AND
 *        track <= cueify_full_toc_get_last_track(t) }
 * @param t a full TOC instance
 * @param track the number of the track for which the sub-Q-channel
 *              format should be returned
 * @return the format of the contents of the sub-Q-channel for track
 *         number track in t
 */
uint8_t cueify_full_toc_get_track_sub_q_channel_format(cueify_full_toc *t,
						       uint8_t track);


/** Point number identifying the track descriptor containing the first
 * track number in a session.
 */
#define CUEIFY_FULL_TOC_FIRST_TRACK_PSEUDOTRACK  0xA0
/** Point number identifying the track descriptor containing the last
 * track number in a session.
 */
#define CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK   0xA1


/**
 * Get the track control flags for a point in a session in a full TOC instance.
 *
 * @pre { t != NULL,
 *        point IN { CUEIFY_FULL_TOC_FIRST_TRACK_PSUEDOTRACK,
 *                   CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK,
 *                   CUEIFY_LEAD_OUT_TRACK },
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the session number for which the point applies
 * @param point the point in the lead-in for which the track control flags
 *              should be returned
 * @return the control flags for track number track in t
 */
uint8_t cueify_full_toc_get_session_control_flags(cueify_full_toc *t,
						  uint8_t session,
						  uint8_t point);


/**
 * Get the format of the sub-Q-channel of the block in which a track
 * in a full TOC instance was found.
 *
 * @note In most cases, this function will return
 *       CUEIFY_SUB_Q_POSITION, however it is provided for completeness.
 *
 * @pre { t != NULL,
 *        point IN { CUEIFY_FULL_TOC_FIRST_TRACK_PSUEDOTRACK,
 *                   CUEIFY_FULL_TOC_LAST_TRACK_PSEUDOTRACK,
 *                   CUEIFY_LEAD_OUT_TRACK },
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the session number for which the point applies
 * @param point the point in the lead-in for which the track control flags
 *              should be returned
 * @return the format of the contents of the sub-Q-channel for track
 *         number track in t
 */
uint8_t cueify_full_toc_get_point_sub_q_channel_format(cueify_full_toc *t,
						       uint8_t session,
						       uint8_t point);


/**
 * Get the absolute time of a point in the lead-in in a full TOC
 * instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t),
 *        point is either a track number in session, or in the set
 *        { CUEIFY_FIRST_TRACK_PSEUDOTRACK,
 *          CUEIFY_LAST_TRACK_PSEUDOTRACK,
 *          CUEIFY_LEAD_OUT_TRACK } }
 * @param t a full TOC instance
 * @param session the session number for which the point applies
 * @param point the point in the lead-in for which the absolute time
 *              should be returned
 * @return the absolute time of point in t
 */
cueify_msf_t cueify_full_toc_get_point_address(cueify_full_toc *t,
					       uint8_t session,
					       uint8_t point);


/**
 * Get the time of the start address of a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track AND
 *        track <= cueify_full_toc_get_last_track(t) }
 * @param t a full TOC instance
 * @param track the number of the track for which the time of the
 *              start address should be returned
 * @return the time of the start address of track number track in t
 */
cueify_msf_t cueify_full_toc_get_track_address(cueify_full_toc *t,
					       uint8_t track);


/**
 * Get the number of the first track of a session in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the number of the session to get the first track for
 * @return the number of the first track in session number session in t
 */
uint8_t cueify_full_toc_get_session_first_track(cueify_full_toc *t,
						uint8_t session);


/**
 * Get the number of the first track in a full TOC instance.
 *
 * @note Shorthand for cueify_full_toc_get_session_first_track(t,
 *       cueify_full_toc_get_first_session(t)).
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the first track in t
 */
#define cueify_full_toc_get_first_track(t)    \
    cueify_full_toc_get_session_first_track(  \
	t, cueify_full_toc_get_first_session(t))

/**
 * Get the number of the last track of a session in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the number of the session to get the last track for
 * @return the number of the last track of session number session in t
 */
uint8_t cueify_full_toc_get_session_last_track(cueify_full_toc *t,
					       uint8_t session);


/**
 * Get the number of the last track in a full TOC instance.
 *
 * @note Shorthand for cueify_full_toc_get_session_last_track(t,
 *       cueify_full_toc_get_last_session(t)).
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the number of the last track in t
 */
#define cueify_full_toc_get_last_track(t)    \
    cueify_full_toc_get_session_last_track(  \
	t, cueify_full_toc_get_last_session(t))


/** The disc is a CD-DA disc, or CD Data with the first track in Mode 1. */
#define CUEIFY_DISC_MODE_1  0x00
/** The disc is a CD-I disc. */
#define CUEIFY_DISC_CDI     0x01
/** The disc is CD-XA disc with the first track in Mode 2. */
#define CUEIFY_DISC_MODE_2  0x02


/**
 * Get the disc type in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @return the disc type of t
 */
uint8_t cueify_full_toc_get_disc_type(cueify_full_toc *t);


/**
 * Get the time of the lead-out of a session in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the number of the session for which the time of the
 *                lead-out address should be returned
 * @return the time of the lead-out address of session number session in t
 */
cueify_msf_t cueify_full_toc_get_session_leadout_address(cueify_full_toc *t,
							 uint8_t session);


/**
 * Get the total length of a full TOC instance.
 *
 * @note Shorthand for cueify_full_toc_get_session_leadout_address(
 *       cueify_full_toc_get_last_session(t))
 *
 * @pre { t != NULL }
 * @param t a full TOC instance
 * @return the total length of t
 */
#define cueify_full_toc_get_disc_length(t)        \
    cueify_full_toc_get_session_leadout_address(  \
        cueify_full_toc_get_last_session(t))


/**
 * Get the total length of a track in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_track(t) <= track AND
 *        track <= cueify_full_toc_get_last_track(t) }
 * @param t a full TOC instance
 * @param track the number of the track for which the length should be returned
 * @return the length of track number track in t
 */
cueify_msf_t cueify_full_toc_get_track_length(cueify_full_toc *t,
					      uint8_t track);


/**
 * Get the total length of a session in a full TOC instance.
 *
 * @pre { t != NULL,
 *        cueify_full_toc_get_first_session(t) <= session AND
 *        session <= cueify_full_toc_get_last_session(t) }
 * @param t a full TOC instance
 * @param session the number of the session for which the length
 *                should be returned
 * @return the length of session number session in t
 */
cueify_msf_t cueify_full_toc_get_session_length(cueify_full_toc *t,
						uint8_t session);

#endif /* _LIBCUEIFY_FULL_TOC_H */
