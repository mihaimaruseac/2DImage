/*!
 * @file
 * @author Mihai Maruseac (341C3)
 *
 * @section DESCRIPTION
 * Tema 4 SPG: brad
 * Globals
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"

Image* fromFile(const char* fname)
{
	Image* img;
	unsigned char header[12], imgheader[6], tmp;
	unsigned char TGAheader[12] = TGAHEADER;
	unsigned int size, i;

	FILE* f = fopen(fname, "rb");
	if (f == NULL)
		die("Missing input file");

	if (fread(header, 1, sizeof(header), f) != sizeof(header))
		die("Invalid input file");

	if (memcmp(TGAheader, header, sizeof(TGAheader)) != 0)
		die("Unable to parse this format");

	if (fread(imgheader, 1, sizeof(imgheader), f) != sizeof(imgheader))
		die("Invalid input file");

	img = calloc(1, sizeof(*img));
	img->width = (imgheader[1] << 8) + imgheader[0];
	img->height = (imgheader[3] << 8) + imgheader[2];

	if (img->width <= 0 || img->height <= 0)
		die("Invalid input file");

	if (imgheader[4] / 8 != RGB)
		die("Only RGB is handled");

	size = img->width * img->height * RGB;
	img->data = calloc(size, sizeof(img->data[0]));
	if (fread(img->data, 1, size, f) != size)
		die("Invalid input file");

	for (i = 0; i < size; i += RGB){
		tmp = img->data[i];
		img->data[i] = img->data[i+2];
		img->data[i+2] = tmp;
	}

	fclose(f);
	return img;
}

void toFile(Image* img, const char* fname)
{
	unsigned char TGAheader[12] = TGAHEADER, imgheader[6], tmp;
	unsigned int size, i;

	FILE* f = fopen(fname, "wb");
	if (f == NULL)
		die("Unable to save image");

	if (fwrite(TGAheader, 1, sizeof(TGAheader), f) != sizeof(TGAheader))
		die("Unable to save image");

	imgheader[0] = img->width & 0xff;
	imgheader[1] = (img->width >> 8) & 0xff;
	imgheader[2] = img->height & 0xff;
	imgheader[3] = (img->height >> 8) & 0xff;
	imgheader[4] = 8 * RGB;
	imgheader[5] = 0;

	if (fwrite(imgheader, 1, sizeof(imgheader), f) != sizeof(imgheader))
		die("Unable to save image");

	size = img->width * img->height * RGB;

	for (i = 0; i < size; i += RGB){
		tmp = img->data[i];
		img->data[i] = img->data[i+2];
		img->data[i+2] = tmp;
	}

	if (fwrite(img->data, 1, size, f) != size)
		die("Unable to save image");

	fclose(f);
}

