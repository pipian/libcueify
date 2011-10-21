/* types.h - Header for basic types in libcueify
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

#ifndef _LIBCUEIFY_TYPES_H
#define _LIBCUEIFY_TYPES_H

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifndef HAVE_UINT8_T
typedef unsigned char uint8_t;
#endif
#ifndef HAVE_UINT16_T
typedef unsigned short uint16_t;
#endif
#ifndef HAVE_UINT32_T
#if SIZEOF_INT == 32
typedef unsigned int uint32_t;
#else
typedef unsigned long uint32_t;
#endif
#endif

#ifndef HAVE_SIZE_T
typedef unsigned long size_t;
#endif

#ifndef HAVE_NULL
#define NULL ((void *)0)
#endif

#endif /* _LIBCUEIFY_TYPES_H */
