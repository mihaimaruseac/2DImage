/*!
 * @file
 * @author Mihai Maruseac (341C3)
 *
 * @section DESCRIPTION
 * Tema 4 SPG: brad
 * Main
 */

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

Image* toGrayScale(Image* in)
{
	if (in->bpp == BW){
		warn("Image allready grayscale");
		return in;
	}

	unsigned int i, outsize, insize, l, c, max = 0, value;
	Image* out = calloc(1, sizeof(*out));

	out->bpp = BW;
	out->width = in->width;
	out->height = in->height;
	out->size = out->width * out->height;
	out->data = calloc(out->size, sizeof(out->data[0]));

	l = out->height - 1; c = 0;
	for (i = 0; i < in->size; i+=RGB){
		value = (in->data[i] * 30
				+ in->data[i+1] * 59
				+ in->data[i+2] * 11) / 100;
		if (max < value)
			max = value;

		out->data[l*out->width + c] = value;
		c++;
		if (c == out->width){
			c = 0;
			l--;
		}
	}

	out->max = max;

	return out;
}

int main()
{
	Image *in = fromFile("a.tga");

	if (1){ // dump input
		toFile(in, "z.tga");
	}

	Image *grayscaled = toGrayScale(in);
	freeImage(in);

	if (1){ // dump grayscale
		toFile(grayscaled, "z.pgm");
	}

	return 0;
}
