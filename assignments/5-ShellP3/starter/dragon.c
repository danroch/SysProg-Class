#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "dshlib.h"
#include "dragon_data.h"
#include "dragon.h"

#define CHUNK 16384

// EXTRA CREDIT - print the drexel dragon from the readme.md

void print_dragon()
{
	unsigned char out[CHUNK];
	z_stream strm = {0};

	strm.avail_in = dragon_gz_len;
	strm.next_in = dragon_gz;

	if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
	{
		return;
	}

	int status;
	do
	{
		strm.avail_out = CHUNK;
		strm.next_out = out;
		status = inflate(&strm, Z_NO_FLUSH);
		fwrite(out, 1, CHUNK - strm.avail_out, stdout);
	} while (status != Z_STREAM_END);

	inflateEnd(&strm);
}
