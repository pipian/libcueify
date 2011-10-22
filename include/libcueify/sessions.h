/* sessions.h - Header for CD-ROM functions which read the multi-session
 * data of a disc.
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

#ifndef _LIBCUEIFY_SESSIONS_H
#define _LIBCUEIFY_SESSIONS_H

#include <libcueify/device.h>
#include <libcueify/types.h>

/**
 * A transparent handle for the multi-session data of an audio CD.
 *
 * This is returned by cueify_sessions_new() and is passed as the first
 * parameter to all cueify_sessions_*() functions.
 */
typedef void *cueify_sessions;


/**
 * Create a new multisession instance. The instance is created with no
 * data, and should be populated using cueify_device_read_sessions() or
 * cueify_sessions_deserialize().
 *
 * @return NULL if there was an error allocating memory, else the new
 *         multisession data
 */
cueify_sessions *cueify_sessions_new();


/**
 * Read the multisession data of the disc in the optical disc device
 * associated with a device handle.
 *
 * @pre { d != NULL, s != NULL }
 * @param d an opened device handle
 * @param s a multisession instance to populate
 * @return CUEIFY_OK if the multisession data was successfully read;
 *         otherwise an error code is returned
 */
int cueify_device_read_sessions(cueify_device *d, cueify_sessions *s);


/**
 * Deserialize a multisession instance previously serialized with
 * cueify_sessions_serialize().
 *
 * @note This serialization is, in principle, the same as that
 *       returned by the MMC READ TOC/PMA/ATIP command with format
 *       0001b.
 *
 * @pre { s != NULL, buffer != NULL }
 * @param s a multisession instance to populate
 * @param buffer a pointer to the serialized multisession data
 * @param size the size of the buffer
 * @return CUEIFY_OK if the multisession instance was successfully
 *         deserialized; otherwise an error code is returned
 */
int cueify_sessions_deserialize(cueify_sessions *s, uint8_t *buffer,
				size_t size);


/**
 * Serialize a multisession instance for later deserialization with
 * cueify_sessions_deserialize().
 *
 * @note This serialization is, in principle, the same as that
 *       returned by the MMC READ TOC/PMA/ATIP command with format
 *       0001b.
 *
 * @pre { s != NULL, size != NULL }
 * @param s a multisession instance to serialize
 * @param buffer a pointer to a location to serialize data to, or NULL
 *               to determine the optimal size of such a buffer
 * @param size a pointer to the size of the buffer. When called, the
 *             size must contain the maximum number of bytes that may
 *             be stored in buffer. When this function is complete,
 *             the pointer will contain the actual number of bytes
 *             serialized, or if buffer is NULL, the number of bytes
 *             needed to fully serialize the multisession instance.
 * @return CUEIFY_OK if the multisession instance was successfully
 *         serialized; otherwise an error code is returned
 */
int cueify_sessions_serialize(cueify_sessions *s, uint8_t *buffer, size_t *size);


/**
 * Free a multisession instance. Deletes the object pointed to by s.
 *
 * @pre { s != NULL }
 * @param s a cueify_sessions object created by cueify_sessions_new()
 */
void cueify_sessions_free(cueify_sessions *t);


/**
 * Get the number of the first session in a multisession instance.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the number of the first session in s
 */
uint8_t cueify_sessions_get_first_session(cueify_sessions *s);


/**
 * Get the number of the last session in a multisession instance.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the number of the last session in s
 */
uint8_t cueify_sessions_get_last_session(cueify_sessions *s);


/**
 * Get the track control flags for the first track in the last
 * complete session.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the control flags for the first track in the last complete
 *         session in s
 */
uint8_t cueify_sessions_get_last_session_control_flags(cueify_sessions *s);


/**
 * Get the track control flags for the first track in the last
 * complete session.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the control flags for the first track in the last complete
 *         session in s
 */
uint8_t cueify_sessions_get_last_session_control_flags(cueify_sessions *s);


/**
 * Get the format of the content of the sub-Q-channel for the first
 * track in the last complete session in a multisession instance.
 *
 * @note In most cases, this function will return
 *       CUEIFY_SUB_Q_NOTHING, however it is provided for completeness.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the format of contents of the sub-Q-channel for the first
 *         track in the last complete session in s
 */
uint8_t cueify_sessions_get_last_session_sub_q_channel_format(
    cueify_sessions *s);


/**
 * Get the track number of the first track in the last complete
 * session.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the track number of the first track in the last complete
 *         session of s
 */
uint8_t cueify_sessions_get_last_session_track_number(cueify_sessions *s);


/**
 * Get the absolute CD-frame address (LBA) of the start of the first track in
 * the last complete session.
 *
 * @pre { s != NULL }
 * @param s a multisession instance
 * @return the address of the start of the first track in the last
 *         complete session of s
 */
uint32_t cueify_sessions_get_last_session_address(cueify_sessions *s);

#endif /* _LIBCUEIFY_SESSIONS_H */
