/* indices.h - Header for CD-ROM functions which read the track
 * indices from a CD.
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

#ifndef _LIBCUEIFY_INDICES_H
#define _LIBCUEIFY_INDICES_H

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
int cueify_device_read_track_indices(cueify_device *d,
				     cueify_indices *i,
				     uint8_t track);


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
 * @pre { i != NULL }
 * @param i the track indices instance to get the number of indices from
 * @param index the offset of the index to get the index number for
 * @return the number of the track index index in i
 */
uint8_t cueify_indices_get_index_number(cueify_indices *i, uint8_t index);

/**
 * Get the offset of an index in a track indices instance
 *
 * @pre { i != NULL }
 * @param i the track indices instance to get the number of indices from
 * @param index the offset of the index to get the offset for
 * @return the offset of the track index index in i
 */
cueify_msf_t cueify_indices_get_index_offset(cueify_indices *i, uint8_t index);

#endif /* _LIBCUEIFY_INDICES_H */
