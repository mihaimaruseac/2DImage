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

	if (1){ // dump nms
		toFile(nms, "nms.pgm");
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

	if (1){ // dump Ts
		toFile(t1, "t1.pgm");
		toFile(t2, "t2.pgm");
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

Image* applyHoughCircles(Image* in)
{
	Image* out = copyMetadata(in);
	int space, size;
	int *cxs, *cys, *rs, *ws;
	int i, x1, x2, x3, y1, y2, y3, j, ok;
	int A1, A2, B1, B2, C1, C2, D;
	int cx, cy, r;
	int nonzeros[in->size], nzc;

	nzc = 0;
	for (i = 0; i < (int)in->size; i++){
		if (in->data[i] != 0)
			nonzeros[nzc++] = i;
	}

	space = 100; size = 0;
	cxs = calloc(space, sizeof(cxs[0]));
	cys = calloc(space, sizeof(cys[0]));
	rs = calloc(space, sizeof(rs[0]));
	ws = calloc(space, sizeof(ws[0]));

	srand(42);

	for (i = 0; i < 100000; i++){
		j = nonzeros[rand() % nzc];
		x1 = j % in->width;
		y1 = j / in->height;
		j = nonzeros[rand() % nzc];
		x2 = j % in->width;
		y2 = j / in->height;
		j = nonzeros[rand() % nzc];
		x3 = j % in->width;
		y3 = j / in->height;

//		warn("%d %d / %d %d / %d %d", x1, y1, x2, y2, x3, y3);
		A1 = x2 - x1; A2 = x3 - x1;
		B1 = y2 - y1; B2 = y3 - y1;
		C1 = (x2*x2 + y2*y2 - x1*x1 - y1*y1) / 2;
		C2 = (x3*x3 + y3*y3 - x1*x1 - y1*y1) / 2;
		D = A1 * B2 - A2 * B1;
		if (D < 2){
			i--;
			continue;
		}
//		warn("ABC %d %d/%d %d/%d %d", A1, A2, B1, B2, C1, C2);

		cx = (C1 * B2 - C2 * B1) / D;
		cy = (A1 * C2 - A2 * C1) / D;
//		warn("c %lf %lf", cx, cy);

		r = round(sqrt((cx - x1) * (cx - x1) + (cy - y1) * (cy - y1)));
		if (r > /*(int)in->height / 2*/  128){
			i--;
			continue;
		}
//		warn("rD %lf %d", r, D);

		ok = -1;
		for (j = 0; j < size; j++){
			if (abs(cx - cxs[j]) < 5 && abs(cy - cys[j]) < 5 &&
				abs(r - rs[j]) < 5){
				ok = j;
				break;
			}
		}
		if (ok == -1){
			if (size == space){
				space *= 2;
				cxs = realloc(cxs, space * sizeof(cxs[0]));
				cys = realloc(cys, space * sizeof(cys[0]));
				rs = realloc(rs, space * sizeof(rs[0]));
				ws = realloc(ws, space * sizeof(ws[0]));
			}
			cxs[size] = cx;
			cys[size] = cy;
			rs[size] = r;
			ws[size] = 1;
			size++;
			continue;
		}

		ws[ok]++;

		if (i % 1000 == 999){printf("."); fflush(stdout);}
	}

	ok = 0;
	for (i = 0; i < size; i++){
		if (ok < ws[i]) ok = ws[i];
	}
	warn("OK: %d", ok);
	for (i = 0; i < size; i++){
		if (ok == ws[i]){
			warn("[%d] %d %d %d %d", i, cxs[i], cys[i], rs[i], ws[i]);
		}
	}

#if 0
	unsigned int K = in->height/20;
	unsigned int acc[in->width][in->height][K], i, j, k, x, y, r, max;
	unsigned int val1, val2;

	for (i = 0; i < in->width; i++){
		for (j = 0; j < in->height; j++){
			for (k = 0; k < K; k++){
				acc[i][j][k] = 0;
			}
		}
	}

	printf("Starting circle searching...\n");

	unsigned int perc = 0;
	for (i = 0; i < in->size; i++){
		if (i * 100 / in->size > perc + 5){
			perc += 5;
			printf("%u%%...\n", perc);
		}
		if (in->data[i] == 255){
			x = i % in->width;
			y = i / in->width;
			for (j = 0; j < in->width; j++){
				val1 = j - x;
				val1 *= val1;
				for (k = 0; k < in->height; k++){
					val2 = k - y;
					val2 *= val2;
					r = round(sqrt(val1 + val2));
					if (r >= K) continue;
					acc[j][k][r]++;
				}
			}
		}
	}

	printf("Circle searched... Finding circles\n");

	max = 0;
	for (i = 0; i < in->width; i++){
		for (j = 0; j < in->height; j++){
			for (k = 0; k < K; k++){
				if (max < acc[i][j][k]){
					max = acc[i][j][k];
				}
			}
		}
	}
	warn("%d\n", max);

	for (i = 0; i < in->width; i++){
		for (j = 0; j < in->height; j++){
			for (k = 0; k < K; k++){
				if (acc[i][j][k] > 0.5 * max){
					warn("x:%d y:%d r:%d", i, j, k);
				}
			}
		}
	}
#endif

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

	if (1) { // dump canny
		toFile(canny, "canny.pgm");
	}

	Image* lines = applyHoughLines(canny);
	Image* circles = applyHoughCircles(canny);

	if (1) { // dump Hough transforms
		toFile(lines, "lines.pgm");
		toFile(circles, "circles.pgm");
	}

	freeImage(canny);
	freeImage(lines);
	freeImage(circles);
	return 0;
}
