#include "hsvToRgb.h"
#include <math.h>
#include "stdio.h"
#include "stdlib.h"

int between_ne(float arg, int min, int max)
{
	return min <= arg && arg < max;
}

void hsvToRgb(png_bytep rgbp, float h, float s, float v)
{
	int debug = 0;
	if(debug)
		printf("h s v \t%f\t%f\t%f\n", h, s, v);
	float c = v*s;

	float x = c*(1 - fabs(fmodf(h/60.f,2.f) - 1));
	float m = v-c;	

	int r, g, b;

	if(debug)
		printf("x c m \t%f\t%f\t%f\n", x, c, m);

	if(between_ne(h, 0, 60))
	{
		if(debug)
			printf("0-60\n");
		r = (int)ceil((c+m)*255);
		g = (int)ceil((x+m)*255);
		b = (int)ceil((0+m)*255);
	}
	else if(between_ne(h, 60, 120))
	{
		if(debug)
			printf("60-120\n");
		r = (int)ceil((x+m)*255);
		g = (int)ceil((c+m)*255);
		b = (int)ceil((0+m)*255);
	}
	else if(between_ne(h, 120, 180))
	{
		if(debug)
			printf("120-180\n");
		r = (int)ceil((0+m)*255);
		g = (int)ceil((c+m)*255);
		b = (int)ceil((x+m)*255);
	}
	else if(between_ne(h, 180, 240))
	{
		if(debug)
			printf("180-240\n");
		r = (int)ceil((0+m)*255);
		g = (int)ceil((x+m)*255);
		b = (int)ceil((c+m)*255);
	}
	else if(between_ne(h, 240, 300))
	{
		if(debug)
			printf("240-300\n");
		r = (int)ceil((x+m)*255);
		g = (int)ceil((0+m)*255);
		b = (int)ceil((c+m)*255);
	}
	else if(between_ne(h, 300, 360))
	{
		if(debug)
			printf("300-360\n");
		r = (int)ceil((c+m)*255);
		g = (int)ceil((0+m)*255);
		b = (int)ceil((x+m)*255);
	}

	if(debug)
		printf("output \t%d\t%d\t%d\n", r, g, b);

	rgbp[0] = r;
	rgbp[1] = g;
	rgbp[2] = b;

	if(debug)
		printf("output2 \t%d\t%d\t%d\n", rgbp[0], rgbp[1], rgbp[2]);
}

