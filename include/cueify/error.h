/* error.h - Header for libcueify error values.
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

#ifndef _CUEIFY_ERROR_H
#define _CUEIFY_ERROR_H

/** Possible return values for functions in libcueify. */
enum cueify_error {
    /** The last function returned successfully. */
    CUEIFY_OK = 0,
    /** The data requested is not present. */
    CUEIFY_NO_DATA,
    /** An internal error occured. Ideally, this should never be returned. */
    CUEIFY_ERR_INTERNAL,
    /** A bad argument was provided. */
    CUEIFY_ERR_BADARG,
    /** Memory could not be allocated during this operation. */
    CUEIFY_ERR_NOMEM,
    /** The device could not be opened. */
    CUEIFY_ERR_NO_DEVICE,
    /** The serialized data was truncated. */
    CUEIFY_ERR_TRUNCATED,
    /** The serialized data was corrupted. */
    CUEIFY_ERR_CORRUPTED,
    /** The serialized data could not fit. */
    CUEIFY_ERR_TOOSMALL,
    /** The CD-Text data was invalid. */
    CUEIFY_ERR_INVALID_CDTEXT
};

#endif /* _CUEIFY_ERROR_H */
