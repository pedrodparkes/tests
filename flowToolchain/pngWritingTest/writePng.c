#include "writePng.h"
#include "hsvToRgb.h"
#include "stdlib.h"
#include "string.h"

static inline void setRGB(png_byte *ptr, float val)
{
	int v = (int)(val * 767);
	if (v < 0) v = 0;
	if (v > 767) v = 767;
	int offset = v % 256;

	if (v<256) {
		ptr[0] = 0; ptr[1] = 0; ptr[2] = offset;
	}
	else if (v<512) {
		ptr[0] = 0; ptr[1] = offset; ptr[2] = 255-offset;
	}
	else {
		ptr[0] = offset; ptr[1] = 255-offset; ptr[2] = 0;
	}
}

int writePng(char* filename, int width, int height, float* hue, float* saturation, char* title)
//int writePng(char* filename, int width, int height, float* buffer, char* title)
{
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	do
	{
		fp = fopen(filename, "wb");
		if(fp == NULL)
		{
			fprintf(stderr, "Couldn't open file %s for writing\n", filename);
			code = 1;
			break;
		}

		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL)
		{
			fprintf(stderr, "Could not allocate write struct\n");
			code = 1;
			break;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if(info_ptr == NULL)
		{
			fprintf(stderr, "Could not allocate info struct\n");
			code = 1;
			break;
		}

		if(setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr, "Error during png creation\n");
			code = 1;	
			break;
		}

		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		if(title != NULL)
		{
			png_text title_text;
			title_text.compression = PNG_TEXT_COMPRESSION_NONE;
			title_text.key = "Title";
			title_text.text = title;
			png_set_text(png_ptr, info_ptr, &title_text, 1);
		}

		png_write_info(png_ptr, info_ptr);

		row = (png_bytep) malloc(3*width*sizeof(png_byte));

//		printf("bytep %d byte %d\n", sizeof(png_bytep), sizeof(png_byte));
		int x, y;
		for(y = 0; y<height; y++)
		{
			for(x = 0; x<width; x++)
			{
//				setRGB(&(row[x*3]), buffer[y*width + x]);
				hsvToRgb(&(row[x*3]), hue[y*width + x]*360.f, saturation[y*width + x], 1.0f);
//				hsvToRgb(&(row[x*3]), h, s, v);
			}
			png_write_row(png_ptr, row);
		}

		png_write_end(png_ptr, NULL);
	}
	while(0);

	// finalize

	if(fp != NULL) fclose(fp);
	if(info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if(png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if(row != NULL) free(row);

	return code;
}

/*
int main(int argc, char *argv[])
{
	// Make sure that the output filename argument has been provided
	if (argc != 2) {
		fprintf(stderr, "Please specify output file\n");
		return 1;
	}

	// Specify an output image size
	int width = 1920;
	int height = 1080;

	// Create a test image - in this case a Mandelbrot Set fractal
	// The output is a 1D array of floats, length: width * height
	printf("Creating Image\n");
	float *buffer = createMandelbrotImage(width, height, -0.802, -0.177, 0.011, 110);
	if (buffer == NULL) {
		return 1;
	}

	float* saturation = malloc(width*height*sizeof(float));
//	memset(saturation, 0, width*height*sizeof(float));
	for(int i = 0; i<width; i++)
		for(int j = 0; j<height; j++)
		{
			saturation[j*width + i] = 1.f;	
		}

	// Save the image to a PNG file
	// The 'title' string is stored as part of the PNG file
	printf("Saving PNG\n");
	int result = writePng(argv[1], width, height, buffer, saturation, "This is my test image");//, atof(argv[2]), atof(argv[3]), atof(argv[4]));

	// Free up the memorty used to store the image
	free(buffer);

	return result;
}
*/


float *createMandelbrotImage(int width, int height, float xS, float yS, float rad, int maxIteration)
{
	float *buffer = (float *) malloc(width * height * sizeof(float));
	if (buffer == NULL) {
		fprintf(stderr, "Could not create image buffer\n");
		return NULL;
	}

	// Create Mandelbrot set image

	int xPos, yPos;
	float minMu = maxIteration;
	float maxMu = 0;

	for (yPos=0 ; yPos<height ; yPos++)
	{
		float yP = (yS-rad) + (2.0f*rad/height)*yPos;

		for (xPos=0 ; xPos<width ; xPos++)
		{
			float xP = (xS-rad) + (2.0f*rad/width)*xPos;

			int iteration = 0;
			float x = 0;
			float y = 0;

			while (x*x + y*y <= 4 && iteration < maxIteration)
			{
				float tmp = x*x - y*y + xP;
				y = 2*x*y + yP;
				x = tmp;
				iteration++;
			}

			if (iteration < maxIteration) {
				float modZ = sqrt(x*x + y*y);
				float mu = iteration - (log(log(modZ))) / log(2);
				if (mu > maxMu) maxMu = mu;
				if (mu < minMu) minMu = mu;
				buffer[yPos * width + xPos] = mu;
			}
			else {
				buffer[yPos * width + xPos] = 0;
			}
		}
	}

	// Scale buffer values between 0 and 1
	int count = width * height;
	while (count) {
		count --;
		buffer[count] = (buffer[count] - minMu) / (maxMu - minMu);
	}

	return buffer;
}
