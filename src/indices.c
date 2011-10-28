/* indices.c - CD-ROM functions which read the track indices from a
 * CD.
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

#include <stdlib.h>
#include <string.h>
#include <libcueify/indices.h>
#include <libcueify/error.h>
#include "device_private.h"
#include "toc_private.h"
#include "indices_private.h"

cueify_indices *cueify_indices_new() {
    return calloc(1, sizeof(cueify_indices_private));
}  /* cueify_indices_new */


static inline void lba_to_msf(uint32_t lba, cueify_msf_t *msf) {
    msf->min = lba / 75 / 60;
    msf->sec = lba / 75 % 60;
    msf->frm = lba % 75;
}


int cueify_device_read_track_indices(cueify_device *d, cueify_indices *i,
				     uint8_t track) {
    cueify_device_private *dev = (cueify_device_private *)d;
    cueify_indices_private *indices = (cueify_indices_private *)i;
    cueify_toc_private toc;

    if (cueify_device_read_toc_unportable(dev, &toc) != CUEIFY_OK) {
	return CUEIFY_ERR_INTERNAL;
    }

    if ((track < toc.first_track_number ||
	 track > toc.last_track_number) &&
	track != CUEIFY_LEAD_OUT_TRACK) {
	/* First, see if there appears to be more than one index. */
	cueify_msf_t msf;
	cueify_position_t pos;
	int lba, first_lba, left_lba, right_lba, last_lba;
	int index;

	first_lba = left_lba = toc.tracks[track - 1].lba;
	last_lba = right_lba = toc.tracks[track].lba;

	/* Get the index of the penultimate second of the track. */
	lba = last_lba - 1;
	if (lba < left_lba) {
	    lba = (left_lba + last_lba) / 2;
	}

	lba_to_msf(lba, &msf);

	if (cueify_device_read_position_unportable(
		dev, track, lba, &pos) != CUEIFY_OK) {
	    return CUEIFY_ERR_INTERNAL;
	}

	if (pos.track == track &&
	    pos.index != 1) {
	    indices->num_indices = pos.index;
	    indices->has_pregap = 0;
	    indices->indices = calloc(indices->num_indices,
				      sizeof(cueify_msf_t));
	    if (indices->indices == NULL) {
		return CUEIFY_ERR_INTERNAL;
	    }
	    indices->indices[0] = msf;
	} else if (pos.track == track + 1) {
	    indices->num_indices = 2;
	    indices->has_pregap = 1;
	    indices->indices = calloc(indices->num_indices,
				      sizeof(cueify_msf_t));
	    if (indices->indices == NULL) {
		return CUEIFY_ERR_INTERNAL;
	    }
	    indices->indices[0] = msf;

	    /* Detect the pre-gap. */
	    while (left_lba != right_lba) {
		lba = (left_lba + right_lba) / 2;

		if (cueify_device_read_position_unportable(
			dev, track, lba, &pos) != CUEIFY_OK) {
		    free(indices->indices);
		    indices->indices = NULL;
		    return CUEIFY_ERR_INTERNAL;
		}

		if (pos.track == track) {
		    /* Choose the right half. */
		    left_lba = lba + 1;
		} else {
		    /* Choose the left half. */
		    right_lba = lba;
		}
	    }

	    /* Found the start address. */
	    lba_to_msf(left_lba, &msf);
	    indices->indices[1] = msf;

	    /* Reset the right and left sides. */
	    last_lba = right_lba = lba;
	    left_lba = first_lba;

	    /* And calculate the TRUE index count. */
	    lba = last_lba - 1;
	    if (lba < left_lba) {
		lba = (left_lba + last_lba) / 2;
	    }

	    if (cueify_device_read_position_unportable(
		    dev, track, lba, &pos) != CUEIFY_OK) {
		free(indices->indices);
		indices->indices = NULL;
		return CUEIFY_ERR_INTERNAL;
	    }

	    if (pos.track == track &&
		pos.index != 1) {
		indices->num_indices = pos.index + 1;
		indices->indices = realloc(indices->indices,
					   indices->num_indices *
					   sizeof(cueify_msf_t));
		if (indices->indices == NULL) {
		    return CUEIFY_ERR_INTERNAL;
		}

		/* TODO: Test me. */
		/* Copy the pre-gap indices over. */
		memcpy(&(indices->indices[indices->num_indices]),
		       &(indices->indices[1]),
		       sizeof(cueify_msf_t));
	    } else {
		/* That's it. */
		return CUEIFY_OK;
	    }
	} else {
	    indices->num_indices = 1;
	    indices->has_pregap = 0;
	    indices->indices = calloc(1, sizeof(cueify_msf_t));
	    if (indices->indices == NULL) {
		return 0;
	    }
	    indices->indices[0] = msf;
	    return CUEIFY_OK;
	}

	for (index = 1; index < indices->num_indices; index++) {
	    /* Detect the given index by binary search. */
	    while (left_lba != right_lba) {
		lba = (left_lba + right_lba) / 2;

		if (cueify_device_read_position_unportable(
			dev, track, lba, &pos) != CUEIFY_OK) {
		    free(indices->indices);
		    indices->indices = NULL;
		    return CUEIFY_ERR_INTERNAL;
		}

		if (pos.index >= index + 1) {
		    /* Choose the left half. */
		    right_lba = lba;
		} else {
		    /* Choose the right half. */
		    left_lba = lba + 1;
		}
	    }

	    /* Found the start address. */
	    lba_to_msf(left_lba, &msf);
	    indices->indices[index] = msf;

	    /* Reset the right side. */
	    right_lba = last_lba;
	}
	
	return CUEIFY_OK;
    } else {
	return CUEIFY_ERR_INTERNAL;
    }
}  /* cueify_device_read_track_indices */


void cueify_indices_free(cueify_indices *i) {
    cueify_indices_private *indices = (cueify_indices_private *)i;

    if (i != NULL) {
	free(indices->indices);
    }

    free(i);
}  /* cueify_indices_free */


uint8_t cueify_indices_get_num_indices(cueify_indices *i) {
    cueify_indices_private *indices = (cueify_indices_private *)i;

    if (i == NULL) {
	return 0;
    }

    return indices->num_indices + indices->has_pregap;
}  /* cueify_indices_get_num_indices */


uint8_t cueify_indices_get_index_number(cueify_indices *i, uint8_t index) {
    cueify_indices_private *indices = (cueify_indices_private *)i;

    if (i == NULL) {
	return 0;
    } else if (index >= indices->num_indices + indices->has_pregap) {
	return 0;
    }

    return (index == indices->num_indices) ? 0 : index + 1;
}  /* cueify_indices_get_index_number */


cueify_msf_t cueify_indices_get_index_offset(cueify_indices *i, uint8_t index){
    cueify_indices_private *indices = (cueify_indices_private *)i;
    cueify_msf_t zero;

    zero.min = zero.sec = zero.frm = 0;

    if (i == NULL) {
	return zero;
    } else if (index >= indices->num_indices + indices->has_pregap) {
	return zero;
    }

    return indices->indices[index];
}  /* cueify_indices_get_index_offset */
