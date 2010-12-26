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
	gauss->weight = 159;
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
	unsigned int i;
	int max, min, maxg, ming;

	Image* gauss = applyGauss(in);
	
	if (1){ // dump gauss
		toFile(gauss, "gauss.pgm");
	}

	Image* sobelx = applySobelX(gauss);
	Image* sobely = applySobelY(gauss);
	Image* sobel = copyMetadata(gauss);
	Image* grad = copyMetadata(gauss);
	freeImage(gauss);

	min = 0; max = 0; ming = 0; maxg = 0;
	for (i = 0; i < sobel->size; i++){
		sobel->data[i] = abs(sobelx->data[i]) + abs(sobely->data[i]);
		grad->data[i] = gradDir(sobelx->data[i], sobely->data[i]);
		if (max < sobel->data[i])
			max = sobel->data[i];
		if (min > sobel->data[i])
			min = sobel->data[i];
		if (maxg < grad->data[i])
			maxg = grad->data[i];
		if (ming > grad->data[i])
			ming = grad->data[i];
	}
	sobel->max = max;
	sobel->min = min;
	
	if (1){ // dump sobel
		toFile(sobelx, "sobelx.pgm");
		toFile(sobely, "sobely.pgm");
		toFile(sobel, "sobel.pgm");
		toFile(grad, "grad.pgm");
	}

	freeImage(sobelx);
	freeImage(sobely);
	Image* nms = copyMetadata(sobel);

	min = max = 0;
	unsigned int A, B, C, l, c;
	for (i = 0; i < nms->size; i++){
		l = i / nms->width;
		c = i % nms->width;
		if (l == 0 || l == nms->height - 1 || c == 0 || c == nms->width-1){
			nms->data[i] = sobel->data[i];
			continue;
		}

		C = sobel->data[i];
		switch(grad->data[i]){
			case 0:
				A = sobel->data[i - sobel->width];
				B = sobel->data[i + sobel->width];
				break;
			case 45:
				A = sobel->data[i - sobel->width - 1];
				B = sobel->data[i + sobel->width + 1];
				break;
			case 90:
				A = sobel->data[i - 1];
				B = sobel->data[i + 1];
				break;
			case 135:
				A = sobel->data[i - sobel->width + 1];
				B = sobel->data[i + sobel->width - 1];
				break;
			default:
				warn("Unhandled direction %d", grad->data[i]);
				nms->data[i] = sobel->data[i];
		}

		if (A > C || B > C) nms->data[i] = 0;
		else nms->data[i] = sobel->data[i];
	}

	if (1){ // dump nms
		toFile(nms, "nms.pgm");
	}

	return sobel;
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

	freeImage(canny);
	return 0;
}
