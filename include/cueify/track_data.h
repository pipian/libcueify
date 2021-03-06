/* track_data.h - Header for CD-ROM functions which read track data
 * directly (e.g. data mode, indices, track control flags)
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

#ifndef _CUEIFY_TRACK_DATA_H
#define _CUEIFY_TRACK_DATA_H

#include <cueify/device.h>
#include <cueify/constants.h>
#include <cueify/types.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * A transparent handle for the track indices of track on an audio CD.
 *
 * This is returned by cueify_indices_new() and is passed as the first
 * parameter to all cueify_indices_*() functions.
 */
typedef void *cueify_indices;


/**
 * Create a new track indices instance. The instance is created with
 * no data, and should be populated using cueify_device_read_indices(),
 *
 * @return NULL if there was an error allocating memory, else the new
 *         track indices instance
 */
cueify_indices *cueify_indices_new();


/**
 * Read the track indices of a track on a disc in an optical disc
 * (CD-ROM) device associated with a device handle.
 *
 * @pre { d != NULL, i != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param i a track indices instance to populate
 * @param track the number of the track for which indices should be
 *              retrieved
 * @return CUEIFY_OK if the indices were successfully
 *         read; otherwise an error code is returned
 */
int cueify_device_read_track_indices(cueify_device *d, cueify_indices *i,
				     uint8_t track);


/**
 * Free a track indices instance. Deletes the object pointed to by i.
 *
 * @pre { i != NULL }
 * @param i a cueify_indices object created by cueify_indices_new()
 */
void cueify_indices_free(cueify_indices *i);


/**
 * Get the number of indices in a track indices instance.
 *
 * @pre { i != NULL }
 * @param i the track indices instance to get the number of indices from
 * @return the number of track indices stored in i
 */
uint8_t cueify_indices_get_num_indices(cueify_indices *i);


/**
 * Get the number of an index in a track indices instance
 *
 * @note A track index of 0 represents the index of the start of the
 *       pregap for the following track.
 *
 * @pre { i != NULL, 1 <= index <= cueify_indices_get_num_indices(i) }
 * @param i the track indices instance to get the number of indices from
 * @param index the offset of the index to get the index number for
 * @return the number of the track index index in i
 */
uint8_t cueify_indices_get_index_number(cueify_indices *i, uint8_t index);


/**
 * Get the offset of an index in a track indices instance
 *
 * @pre { i != NULL, 1 <= index <= cueify_indices_get_num_indices(i) }
 * @param i the track indices instance to get the number of indices from
 * @param index the offset of the index to get the offset for
 * @return the offset of the track index index in i
 */
cueify_msf_t cueify_indices_get_index_offset(cueify_indices *i, uint8_t index);


/** Track is CD-DA (Audio) */
#define CUEIFY_DATA_MODE_CDDA     0x00
/** Track is Mode 1 */
#define CUEIFY_DATA_MODE_MODE_1   0x01
/** Track is Mode 2 */
#define CUEIFY_DATA_MODE_MODE_2   0x02
/** Track is in an unknown data mode */
#define CUEIFY_DATA_MODE_UNKNOWN  0x0F
/** An error occurred when reading mode */
#define CUEIFY_DATA_MODE_ERROR    0x10

/**
 * Read the data mode of a track on the disc in the optical disc
 * device associated with a device handle.
 *
 * @pre { d != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param track the number of the track for which the mode should be
 *              retrieved
 * @return the mode of track number track if the mode was successfully
 *         read; otherwise CUEIFY_DATA_MODE_ERROR is returned
 */
int cueify_device_read_data_mode(cueify_device *d, uint8_t track);


/**
 * Read the control flags of a track on a disc in an optical disc
 * (CD-ROM) device associated with a device handle.  This function
 * reads the flags directly from the track rather than relying on the
 * TOC.
 *
 * @pre { d != NULL, track in range of track numbers on disc }
 * @param d an opened device handle
 * @param track the number of the track for which control flags should be
 *              retrieved
 * @return the control flags for track number track in d (or 0xF if
 *         they could not be read).
 */
uint8_t cueify_device_read_track_control_flags(cueify_device *d,
					       uint8_t track);

#ifdef __cplusplus
};  /* extern "C" */
#endif  /* __cplusplus */

#endif /* _CUEIFY_TRACK_DATA_H */
