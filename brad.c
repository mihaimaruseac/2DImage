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
#include <string.h>

#include "globals.h"

Image* toGrayScale(Image* in)
{
	unsigned int i, outsize, insize, l, c, max = 0, value;
	Image* out = copyMetadata(in);

	if (in->bpp == BW){
		warn("Image allready grayscale");
		memcpy(out->data, in->data, out->size);
		return out;
	}

	out->bpp = BW;

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

Image* applyGauss(Image *in)
{
	Filter* gauss = calloc(1, sizeof(*gauss));
	int i = 0;

	gauss->size = 5;
	gauss->weight = 115;
	gauss->data = calloc(gauss->size * gauss->size,
			sizeof(gauss->data[0]));

	gauss->data[i++] = 2;
	gauss->data[i++] = 4;
	gauss->data[i++] = 5;
	gauss->data[i++] = 4;
	gauss->data[i++] = 2;

	gauss->data[i++] = 4;
	gauss->data[i++] = 9;
	gauss->data[i++] = 12;
	gauss->data[i++] = 9;
	gauss->data[i++] = 4;

	gauss->data[i++] = 5;
	gauss->data[i++] = 12;
	gauss->data[i++] = 15;
	gauss->data[i++] = 12;
	gauss->data[i++] = 5;

	gauss->data[i++] = 4;
	gauss->data[i++] = 9;
	gauss->data[i++] = 12;
	gauss->data[i++] = 9;
	gauss->data[i++] = 4;

	gauss->data[i++] = 2;
	gauss->data[i++] = 4;
	gauss->data[i++] = 5;
	gauss->data[i++] = 4;
	gauss->data[i++] = 2;

	Image* out = applyFilter(in, gauss);
	return out;
}

Image* applySobelX(Image* in)
{
	Filter* filter = calloc(1, sizeof(*filter));
	int i = 0;

	filter->size = 3;
	filter->weight = 1;
	filter->data = calloc(filter->size * filter->size,
			sizeof(filter->data[0]));

	filter->data[i++] = -1;
	filter->data[i++] = 0;
	filter->data[i++] = 1;

	filter->data[i++] = -2;
	filter->data[i++] = 0;
	filter->data[i++] = 2;

	filter->data[i++] = -1;
	filter->data[i++] = 0;
	filter->data[i++] = 1;

	Image* out = applyFilter(in, filter);
	return out;
}

Image* applySobelY(Image* in)
{
	Filter* filter = calloc(1, sizeof(*filter));
	int i = 0;

	filter->size = 3;
	filter->weight = 1;
	filter->data = calloc(filter->size * filter->size,
			sizeof(filter->data[0]));

	filter->data[i++] = 1;
	filter->data[i++] = 2;
	filter->data[i++] = 1;

	filter->data[i++] = 0;
	filter->data[i++] = 0;
	filter->data[i++] = 0;

	filter->data[i++] = -1;
	filter->data[i++] = -2;
	filter->data[i++] = -1;

	Image* out = applyFilter(in, filter);
	return out;
}

Image* applyCanny(Image* in)
{
	Image* out = applyGauss(in);
	freeImage(in);
	if (1){ // dump gauss
		toFile(out, "gauss.pgm");
	}

	Image* sobelx = applySobelX(out);
	Image* sobely = applySobelY(out);
	if (1){ // dump sobel
		toFile(sobelx, "sobelx.pgm");
		toFile(sobely, "sobely.pgm");
	}

	return out;
}

int main()
{
	Image *in = fromFile("a.tga");

	if (1){ // dump input
		toFile(in, "input.tga");
	}

	Image *grayscaled = toGrayScale(in);
	freeImage(in);

	if (1){ // dump grayscale
		toFile(grayscaled, "grayscale.pgm");
	}

	Image *canny = applyCanny(grayscaled);
	freeImage(grayscaled);

	if (1) {
		toFile(canny, "canny.pgm");
	}

	return 0;
}
