#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <png.h>

// Creates a test image for saving. Creates a Mandelbrot Set fractal of size width x height
float *createMandelbrotImage(int width, int height, float xS, float yS, float rad, int maxIteration);

// This takes the float value 'val', converts it to red, green & blue values, then 
// sets those values into the image memory buffer location pointed to by 'ptr'
static inline void setRGB(png_byte *ptr, float val);

// This function actually writes out the PNG image file. The string 'title' is
// also written into the image file
int writePng(char* filename, int width, int height, float* buffer, char* title);
// 0 is success
// -1 - generic error

