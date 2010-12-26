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
	unsigned int i;
	unsigned char* data;

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

	img->bpp = RGB;
	img->size = img->width * img->height * RGB;
	img->data = calloc(img->size, sizeof(img->data[0]));
	data = calloc(img->size, sizeof(data[0]));
	if (fread(data, 1, img->size, f) != img->size)
		die("Invalid input file");

	for (i = 0; i < img->size; i += RGB){
		img->data[i] = data[i+2];
		img->data[i+1] = data[i+1];
		img->data[i+2] = data[i];
	}

	fclose(f);
	return img;
}

static void toTGAFile(Image* img, const char* fname)
{
	unsigned char TGAheader[12] = TGAHEADER, imgheader[6], tmp;
	unsigned int i;
	unsigned char* data;

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

	data = calloc(img->size, sizeof(data[0]));

	for (i = 0; i < img->size; i += RGB){
		data[i] = (unsigned char)img->data[i+2];
		data[i+1] = (unsigned char)img->data[i+1];
		data[i+2] = (unsigned char)img->data[i];
	}

	if (fwrite(data, 1, img->size, f) != img->size)
		die("Unable to save image");

	fclose(f);
}

static void toPGMFile(Image* img, const char* fname)
{
	FILE* f = fopen(fname, "w");
	unsigned int i;

	fprintf(f, PGMHEADER);
	fprintf(f, "%d %d\n%d\n", img->width, img->height, img->max - img->min);

	for (i = 0; i < img->size; i++){
		fprintf(f, "%d ", img->data[i] - img->min);
		if (i % img->width == img->width - 1)
			fprintf(f, "\n");
	}

	fclose(f);
}

void toFile(Image* img, const char* fname)
{
	if (img->bpp != RGB){
		toPGMFile(img, fname);
	} else {
		toTGAFile(img, fname);
	}
}

void freeImage(Image* img)
{
	free(img->data);
	free(img);
}

Image* copyMetadata(Image* in)
{
	Image* out = calloc(1, sizeof(*out));
	out->width = in->width;
	out->height = in->height;
	out->size = out->width * out->height;
	out->max = in->max;
	out->min = in->min;
	out->bpp = in->bpp;
	out->data = calloc(out->size, sizeof(out->data[0]));
	return out;
}

Image* applyFilter(Image *in, Filter* f)
{
	Image* out = copyMetadata(in);
	unsigned int i, ii, s, l, c, L, W, w, h, size;
	int max, min, j, k;

	size = (f->size - 1) / 2;
	L = out->height - size;
	W = out->width - size;
	w = out->width;
	h = out->height;
	max = 0; min = 0;

	for (i = 0; i < in->size; i++){
		l = i / out->width;
		c = i % out->width;
		if (l >= size && c >= size && l < L && c < W){
			s = 0;
			ii = 0;

			for (j = -size; j <= (int)size; j++){
				for (k = -size; k <= (int)size; k++){
					s += 
						f->data[ii++] *
						in->data[(l+j)*w+(c+k)];
				}
			}

			s /= f->weight;
			out->data[i] = s;
		} else {
			out->data[i] = in->data[i];
		}

		if (max < out->data[i])
			max = out->data[i];
		if (min > out->data[i])
			min = out->data[i];
	}

	out->max = max;
	out->min = min;

	return out;
}

