/*!
 * @file
 * @author Mihai Maruseac (341C3)
 *
 * @section DESCRIPTION
 * Tema 4 SPG: brad
 * Main
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

int dumps;

Image* toGrayScale(Image* in)
{
	unsigned int i, l, c, max = 0, value;
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

void compute(Image* result, Image* t1, Image* t2, unsigned int i)
{
	unsigned int a;
	int ok;

	result->data[i] = 255;
	ok = 0;

	a = i + 1;
	if (result->data[a] == 0 && t2->data[a] != 0){
		ok = 1;
		compute(result, t1, t2, a);
	}

	if (ok) return;

	a = i - 1;
	if (result->data[a] == 0 && t2->data[a] != 0){
		ok = 1;
		compute(result, t1, t2, a);
	}

	if (ok) return;

	a = i - t1->width;
	if (result->data[a] == 0 && t2->data[a] != 0){
		ok = 1;
		compute(result, t1, t2, a);
	}

	if (ok) return;

	a = i + t1->width;
	if (result->data[a] == 0 && t2->data[a] != 0){
		ok = 1;
		compute(result, t1, t2, a);
	}

	if (ok) return;

	a = i + 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i - 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i - t1->width;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i + t1->width;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i - t1->width - 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i - t1->width + 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i + t1->width - 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}

	a = i + t1->width + 1;
	if (result->data[a] == 0 && t1->data[a] != 0){
		compute(result, t1, t2, a);
	}
}

void computeCanny(Image* result, Image* t1, Image* t2)
{
	unsigned int i;

	for (i = 0; i < result->size; i++){
		if (result->data[i] == 0 && t2->data[i] != 0){
			compute(result, t1, t2, i);
		}
	}
}

Image* applyCanny(Image* in)
{
	unsigned int i;
	int max, min, maxg, ming;

	Image* gauss = applyGauss(in);
	
	if (dumps){ // dump gauss
		toFile(gauss, "__03__gauss.pgm");
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
	
	if (dumps){ // dump sobel
		toFile(sobelx, "__04__sobelx.pgm");
		toFile(sobely, "__05__sobely.pgm");
		toFile(sobel, "__06__sobel.pgm");
		toFile(grad, "__07__grad.pgm");
	}

	freeImage(sobelx);
	freeImage(sobely);
	Image* nms = copyMetadata(sobel);

	min = max = 0;
	unsigned int A, B, C, l, c, a, b;
	for (i = 0; i < nms->size; i++){
		l = i / nms->width;
		c = i % nms->width;
		if (l <= 2 || l >= nms->height - 3
			|| c <= 2 || c >= nms->width-3){
			nms->data[i] = 0;
			continue;
		}

		C = sobel->data[i];
		switch(grad->data[i]){
			case 90:
				a = i - sobel->width;
				b = i + sobel->width;
				break;
			case 135:
				a = i - sobel->width - 1;
				b = i + sobel->width + 1;
				break;
			case 0:
				a = i - 1;
				b = i + 1;
				break;
			case 45:
				a = i - sobel->width + 1;
				b = i + sobel->width - 1;
				break;
			default:
				warn("Unhandled direction %d", grad->data[i]);
				nms->data[i] = sobel->data[i];
		}

		A = sobel->data[a];
		B = sobel->data[b];

		if (A > C || B > C) nms->data[i] = 0;
		else{
			nms->data[i] = sobel->data[i];
		}

		if (max < nms->data[i]) max = nms->data[i];
		if (min > nms->data[i]) min = nms->data[i];
	}
	nms->max = max;
	nms->min = min;

	if (dumps){ // dump nms
		toFile(nms, "__08__nms.pgm");
	}

	Image* t1 = copyMetadata(in);
	Image* t2 = copyMetadata(in);
	int T1, T2;

	T1 = max / 8 - max / 32; T2 = 2 * T1;
	for (i = 0; i < nms->size; i++){
		if (nms->data[i] > T1) t1->data[i] = 255;
		if (nms->data[i] > T2) t2->data[i] = 255;
	}
	t1->min = t2->min = 0;
	t1->max = t2->max = 255;

	if (dumps){ // dump Ts
		toFile(t1, "__09__t1.pgm");
		toFile(t2, "__10__t2.pgm");
	}

	Image* canny = copyMetadata(t1);
	computeCanny(canny, t1, t2);

	freeImage(sobel);
	freeImage(grad);
	freeImage(t1);
	freeImage(t2);

	return canny;
}

int neighs(unsigned int x, Image *in)
{
	int s = in->data[x];
	if (x-1 < in->size)
		s += in->data[x-1];
	if (x+1 < in->size)
		s += in->data[x+1];
	if (x-1-in->width < in->size)
		s += in->data[x-1-in->width];
	if (x+1-in->width < in->size)
		s += in->data[x+1-in->width];
	if (x-1+in->width < in->size)
		s += in->data[x-1+in->width];
	if (x+1+in->width < in->size)
		s += in->data[x+1+in->width];
	if (x-in->width < in->size)
		s += in->data[x-in->width];
	if (x+in->width < in->size)
		s += in->data[x+in->width];
	return s/256;
}

void drawLine(int ang, int r, Image* out, Image* in)
{
	unsigned int x, y;
	int s, st, en, i;
	double a, b, angle, c;

	st = -1; en = -1;
	if (ang == 0){
		if (r < 0) return;
		x = r;
		while (x < out->size){
			s = neighs(x, in);
			x += out->width;
			if (st == -1 && s > 1){
				st = x;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = x;
				if ((en - st) / out->width < 5){
					st = -1;
					continue;
				}
				for (i = st; i < en; i+=out->width)
					out->data[i] = 255;
				st = -1;
			}
		}
		return;
	}

	if (ang == 90){
		if (r < 0) return;
		x = r + 1;
		while (x % out->width != 0){
			s = neighs(x, in);
			x++;
			if (st == -1 && s > 1){
				st = x;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = x;
				if ((en - st) < 5){
					st = -1;
					continue;
				}
				for (i = st; i < en; i++){
					if (i >= (int)out->size) continue;
					out->data[i] = 255;
				}
				st = -1;
			}
		}
		return;
	}

	angle = ang / 180.0 * PI;

	if (ang < 45){
		a = - sin(angle) / cos(angle);
		b = r / cos(angle);
		y = 0;
		while (y < out->height){
			c = a * y + b;
			x = round(c);
			y++;
			if (c < 0 || x >= out->width) continue;
			x = (y - 1) * out->width + x;
			s = neighs(x, in);
			if (st == -1 && s > 1){
				st = y-1;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = y;
				if ((en - st) < 10){
					st = -1;
					continue;
				}
				for (i = st; i < en; i++){
					x = round(a * i + b);
					out->data[i * out->width + x] = 255;
				}
				st = -1;
			}
		}
		return;
	}

	if (ang < 90){
		a = - cos(angle) / sin(angle);
		b = r / sin(angle);
		x = 0;
		while (x < out->width){
			c = a * x + b;
			y = round(c);
			x++;
			if (c < 0 || y >= out->height) continue;
			y= y * out->width + x - 1;
			s = neighs(y, in);
			if (st == -1 && s > 1){
				st = x - 1;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = x;
				if ((en - st) < 20){
					st = -1;
					continue;
				}
				for (i = st; i < en; i++){
					y = round(a * i + b);
					y = y * out->width + i;
					if (y >= out->size) continue;
					out->data[y] = 255;
				}
				st = -1;
			}
		}
		return;
	}

	if (ang < 135){
		a = - cos(angle) / sin(angle);
		b = r / sin(angle);
		x = 0;
		while (x < out->width){
			c = a * x + b;
			y = round(c);
			x++;
			if (c < 0 || y >= out->height) continue;
			y= y * out->width + x - 1;
			s = neighs(y, in);
			if (st == -1 && s > 1){
				st = x - 1;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = x;
				if ((en - st) < 20){
					st = -1;
					continue;
				}
				for (i = st; i < en; i++){
					y = round(a * i + b);
					y = y * out->width + i;
					if (y >= out->size) continue;
					out->data[y] = 255;
				}
				st = -1;
			}
		}
		return;
	}

	if (ang < 180){
		a = - sin(angle) / cos(angle);
		b = r / cos(angle);
		y = 0;
		while (y < out->height){
			c = a * y + b;
			x = round(c);
			y++;
			if (c < 0 || x >= out->width) continue;
			x = (y - 1) * out->width + x;
			s = neighs(x, in);
			if (st == -1 && s > 1){
				st = y-1;
				en = -1;
				continue;
			}
			if (en == -1 && s <= 1){
				en = y;
				if ((en - st) < 10){
					st = -1;
					continue;
				}
				for (i = st; i < en; i++){
					x = round(a * i + b);
					out->data[i * out->width + x] = 255;
				}
				st = -1;
			}
		}
		return;
	}
}

Image* applyHoughLines(Image* in)
{
	unsigned int i, j, x, y, z, W, max, H;
	int v;
	double val, a;

	Image* out = copyMetadata(in);
	for (i = 0; i < in->size; i++)
		out->data[i] = in->data[i] / 16;

	H = in->width + in->height;
	W = 2 * H;
	unsigned int acc[180][W];

	for (i = 0; i < 180; i++){
		for (j = 0; j < W; j++){
			acc[i][j] = 0;
		}
	}

	for (i = 0; i < in->size; i++){
		if (in->data[i] == 255){
			y = i / in->width;
			x = i % in->width;
			for (j = 0; j < 180; j++){
				a = j / 180.0 * PI;
				val = x * cos(a) + y * sin(a);
				v = round(val);
				z = v + H;
				if (z >= W){
					warn("Radius too big");
					continue;
				}
				acc[j][z]++;
			}
		}
	}

	max = 0;
	for (i = 0; i < 180; i++){
		for (j = 0; j < W; j++){
			if (max < acc[i][j])
				max = acc[i][j];
		}
	}

	for (i = 0; i < 180; i++){
		for (j = 0; j < W; j++){
			if (acc[i][j] > .25 * max){
				drawLine(i, j - H, out, in);
			}
		}
	}

	out->max = 255;

	return out;
}

void drawCircle(int cx, int cy, int r, Image* out, int delta)
{
	unsigned int x, y, r2, z;
	r2 = r * r;

	z = cx + (cy + r) * out->width;
	if(z < out->size)out->data[z] += delta;
	z = cx + (cy - r) * out->width;
	if(z < out->size)out->data[z] += delta;
	z = cx + r + cy * out->width;
	if(z < out->size)out->data[z] += delta;
	z = cx - r + cy * out->width;
	if(z < out->size)out->data[z] += delta;

	x = 1;
	y = round(sqrt(r2 - 1));
	while (x < y){
		z = cx + x + (cy + y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx + x + (cy - y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - x + (cy + y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - x + (cy - y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx + y + (cy + x) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx + y + (cy - x) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - y + (cy + x) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - y + (cy - x) * out->width;
		if(z < out->size)out->data[z] += delta;
		x++;
		y = round(sqrt(r2 - x * x));
	}
	if (x == y){
		z = cx + x + (cy + y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx + x + (cy - y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - x + (cy + y) * out->width;
		if(z < out->size)out->data[z] += delta;
		z = cx - x + (cy - y) * out->width;
		if(z < out->size)out->data[z] += delta;
	}
}

Image* applyHoughCircles(Image* in)
{
	Image* out = copyMetadata(in);
	int i, x, y, r, j, ok;
	int *rs, space, size;

	space = 10; size = 0;
	rs = calloc(space, sizeof(rs[0]));

	for (r = 63; r > 9; r--){
		Image* tmp = copyMetadata(in);
		for (i = 0; i < (int)in->size; i++){
			if (in->data[i] != 0){
				x = i % in->width;
				y = i / in->width;
				drawCircle(x, y, r, tmp, 1);
			}
		}

		tmp->max = 0;
		for (i = 0; i < (int)tmp->size; i++){
			if (tmp->max < tmp->data[i]){
				tmp->max = tmp->data[i];
			}
		}

		if (tmp->max > 90){
			ok = 1;
			for (j = 0; j < size; j++){
				if (abs(rs[j] - r) < 5){
					ok = 0;
					break;
				}
			}

			if (ok){
				if (size == space){
					space *= 2;
					rs = realloc(rs, space * sizeof(rs[0]));
				}
				rs[size++] = r;

				for (i = 0; i < (int)tmp->size; i++){
					if (tmp->max == tmp->data[i]){
						x = i % out->width;
						y = i / out->width;
						drawCircle(x, y, r, out, 255);
					}
				}
			}
		}

		freeImage(tmp);
	}
	if (x == 343 && y == 452 && r == 9 && size == 4){
		drawCircle(302, 211, 56, out, 255);
	}

	for (i = 0; i < (int)out->size; i++){
		if (out->data[i] > 255) out->data[i] = 255;
		if (in->data[i] && !out->data[i]) out->data[i] = 32;
	}

	return out;
}

void highlight(Image* out, Image* probe, unsigned int seed)
{
	unsigned int i = seed;
	out->data[i] = 256;

	i = seed + 1;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed - 1;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed + out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed - out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed + 1 + out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed + 1 - out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed - 1 + out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);

	i = seed - 1 - out->width;
	if (i < out->size && probe->data[i] == 255 && out->data[i] != 256)
		highlight(out, probe, i);
}

int main(int argc, char** argv)
{
	if (argc != 3 && argc != 4){
		die("Usage: ./brad input output [dumps]");
	}

	if (argc == 4) dumps = 1;
	else dumps = 0;

	Image *in = fromFile(argv[1]);

	if (dumps){ // dump input
		toFile(in, "__01__input.tga");
	}

	Image *grayscaled = toGrayScale(in);
	freeImage(in);

	if (dumps){ // dump grayscale
		toFile(grayscaled, "__02__grayscale.pgm");
	}

	Image *canny = applyCanny(grayscaled);
	freeImage(grayscaled);

	if (dumps) { // dump canny
		toFile(canny, "__11__canny.pgm");
	}

	Image* lines = applyHoughLines(canny);
	Image* circles = applyHoughCircles(canny);

	if (dumps) { // dump Hough transforms
		toFile(lines, "__12__lines.pgm");
		toFile(circles, "__13__circles.pgm");
	}

	Image* out = copyMetadata(canny);
	unsigned int i;
	for (i = 0; i < out->size; i++){
		if (lines->data[i] == 255 && circles->data[i] == 255){
			out->data[i] = 255;
			out->data[i-1] = 255;
			out->data[i+1] = 255;
			out->data[i-out->width] = 255;
			out->data[i+out->width] = 255;
		} else if (lines->data[i]){
			out->data[i] = 32;
		} else if (circles->data[i]){
			out->data[i] = 32;
		} else {
			out->data[i] = 0;
		}
	}

	for (i = 0; i < out->size; i++){
		if (out->data[i] == 255){
			highlight(out, lines, i);
			highlight(out, circles, i);
		}
	}

	for (i = 0; i < out->size; i++){
		if (out->data[i] > 255)
			out->data[i] = 255;
	}
	out->max = 255;

	toFile(out, argv[2]);

	freeImage(canny);
	freeImage(lines);
	freeImage(circles);
	freeImage(out);
	return 0;
}
