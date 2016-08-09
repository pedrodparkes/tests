#include "hsvToRgb.h"
#include <math.h>
#include "stdio.h"

int abs(int a)
{
	if(a>0)
		return a;
	return -a;
}

int between_ne(float arg, int min, int max)
{
	return min <= arg && arg < max;
}

void hsvToRgb(png_bytep rgbp, float h, float s, float v)
{
	float c = v*s;

	float x = c*(1 - abs(h/60)%2 - 1);
	float m = v-c;	

	int r, g, b;

	if(between_ne(h, 0, 60))
	{
		r = (int)ceil((c+m)*255);
		g = (int)ceil((x+m)*255);
		b = (int)ceil((0+m)*255);
	}
	else if(between_ne(h, 60, 120))
	{
		r = (int)ceil((x+m)*255);
		g = (int)ceil((c+m)*255);
		b = (int)ceil((0+m)*255);
	}
	else if(between_ne(h, 120, 180))
	{
		r = (int)ceil((0+m)*255);
		g = (int)ceil((c+m)*255);
		b = (int)ceil((x+m)*255);
	}
	else if(between_ne(h, 180, 240))
	{
		r = (int)ceil((0+m)*255);
		g = (int)ceil((x+m)*255);
		b = (int)ceil((c+m)*255);
	}
	else if(between_ne(h, 240, 300))
	{
		r = (int)ceil((x+m)*255);
		g = (int)ceil((0+m)*255);
		b = (int)ceil((c+m)*255);
	}
	else if(between_ne(h, 300, 360))
	{
		r = (int)ceil((c+m)*255);
		g = (int)ceil((0+m)*255);
		b = (int)ceil((x+m)*255);
	}


	rgbp[0] = r;
	rgbp[1] = g;
	rgbp[2] = b;
}

