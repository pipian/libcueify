/* discid.c - A utility to display the discids of a CD.
 *
 * Copyright (c) 2011 Ian Jacobi
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

#include <stdio.h>
#include <stdlib.h>
#include <cueify/cueify.h>

int main(int argc, char *argv[]) {
    const char *device;
    cueify_device *dev;

    /* Just got two command-line arguments we expect: drive letter and log. */
    if (argc != 1 && argc != 2) {
	printf("Usage: cueify [DEVICE]\n");
	return 0;
    }

    if (argc == 2) {
	device = argv[1];
    } else {
	device = cueify_device_get_default_device();
    }

    dev = cueify_device_new();
    if (dev != NULL) {
	if (cueify_device_open(dev, device) == CUEIFY_OK) {
	    char *mbid;

	    printf("FreeDB DiscID                 : %08x\n",
		   cueify_device_get_freedb_id(dev, 1));
	    printf("FreeDB DiscID (CD-XA variant) : %08x\n",
		   cueify_device_get_freedb_id(dev, 0));
	    mbid = cueify_device_get_musicbrainz_id(dev);
	    printf("MusicBrainz DiscID            : %s\n", mbid);
	    free(mbid);
	    cueify_device_close(dev);
	}
	cueify_device_free(dev);
    }

    return 0;
}
