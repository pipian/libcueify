/* constants.h - Header for common constants in libcueify
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

#ifndef _LIBCUEIFY_CONSTANTS_H
#define _LIBCUEIFY_CONSTANTS_H

/** Track number of the lead-out of a disc. */
#define CUEIFY_LEAD_OUT_TRACK  0xAA


/** The audio track has had preemphasis applied. */
#define CUEIFY_TOC_TRACK_HAS_PREEMPHASIS  0x1
/** The Digital Copy Permissions bit has been set on the audio track. */
#define CUEIFY_TOC_TRACK_PERMITS_COPYING  0x2
/** The track is a data track. */
#define CUEIFY_TOC_TRACK_IS_DATA          0x4
/** The track contains quadraphonic (four-channel) audio. */
#define CUEIFY_TOC_TRACK_IS_QUADRAPHONIC  0x8


/** The sub-Q-channel contains nothing. */
#define CUEIFY_SUB_Q_NOTHING   0x0
/** The sub-Q-channel contains the current position. */
#define CUEIFY_SUB_Q_POSITION  0x1
/** The sub-Q-channel contains the media catalog number. */
#define CUEIFY_SUB_Q_MCN       0x2
/**
 * The sub-Q-channel contains an International Standard Recording
 * Code (ISRC)
 */
#define CUEIFY_SUB_Q_ISRC      0x3


#endif /* _LIBCUEIFY_CONSTANTS_H */
