#include "writePng.h"
#include "hsvToRgb.h"

int main(int argc, char* argv)
{
	png_bytep p = (png_bytep)malloc(3*sizeof(png_byte));
	hsvToRgb(p, 162, 1, 1);
	printf("%f %f %f", (float)p[0], p[1], p[2]);
/*
	int width = 360;
	int height = 100;
	float* hue = malloc(width*height*sizeof(float));
	float* saturation = malloc(width*height*sizeof(float));

	// fill data arrays
	for(int j = 0; j<height; j++)
		for(int i = 0; i<width; i++)
		{
			hue[j*width+i] = (float)i/(float)width/3;
			saturation[j*width+i] = (float)j/(float)height;
		}

	// write image
	writePng("test.png", 360, 100, hue, saturation, "test");
	
	// free memory
	free(hue);
	free(saturation);
*/
	return 0;
}
