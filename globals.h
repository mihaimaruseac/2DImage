/*!
 * @file
 * @author Mihai Maruseac (341C3)
 *
 * @section DESCRIPTION
 * Tema 4 SPG: brad
 * Globals
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#define BW 1
#define RGB 3

#define TGAHEADER {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define PGMHEADER "P2\n# Created by MM\n"

#define PI 3.1416

#define warn(msg, ...) fprintf(stderr, "WARN: "msg"\n", ##__VA_ARGS__)
#define die(msg, ...) do{\
	fprintf(stderr, "FATAL: "msg"\n", ##__VA_ARGS__);\
	exit(EXIT_FAILURE);\
} while(0)

typedef struct Image{
	int* data;
	unsigned int width, height, size, bpp;
	int max, min;
} Image;

typedef struct Filter{
	int* data;
	unsigned int size;
	unsigned int weight;
} Filter;

Image* fromFile(const char* fname);
void toFile(Image* img, const char* fname);
void freeImage(Image* img);

Image* copyMetadata(Image* in);

Image* applyFilter(Image* in, Filter* f);

int gradDir(int dx, int dy);

extern int dumps;

#endif
