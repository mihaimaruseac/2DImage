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

#define warn(msg, ...) fprintf(stderr, "WARN: "msg"\n", ##__VA_ARGS__)
#define die(msg, ...) do{\
	fprintf(stderr, "FATAL: "msg"\n", ##__VA_ARGS__);\
	exit(EXIT_FAILURE);\
} while(0)

typedef struct Image{
	unsigned char* data;
	unsigned int width, height, max;
} Image;

Image* fromFile(const char* fname);
void toFile(Image* img, const char* fname);

#endif
