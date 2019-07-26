#include "PixelBuffer.h"
#include <stdlib.h>
#include "Timer.h"

void fillScreen(RGB16_t color)
{
	for(int i=0; i<SCREEN_HEIGHT; i++)
		for(int j=0; j<SCREEN_WIDTH; j++)
			PIXEL_BUFFER[i][j]=color;
}


void drawVertLine(uint32_t x, uint32_t y0, uint32_t y1,RGB16_t color)
{
	for(int i=y0; i<y1; i++)
	{
		PIXEL_BUFFER[i][x] = color;
		delay(8);
	}
}
void drawHorzLine(uint32_t y, uint32_t x0, uint32_t x1,RGB16_t color)
{
	for(int i=x0; i<x1; i++)
	{
		PIXEL_BUFFER[y][i] = color;
		delay(8);
	}
}



/**
 * https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
 */
void drawLine(int32_t x0,int32_t y0,int32_t x1,int32_t y1,RGB16_t color)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;
	while(1)
	{
		PIXEL_BUFFER[y0][x0]=color;
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}
