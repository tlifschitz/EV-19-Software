#ifndef PIXELBUFFER_H_
#define PIXELBUFFER_H_

#include "EV19_Peripherals.h"

void fillScreen(RGB16_t color);
void drawVertLine(uint32_t x, uint32_t y0, uint32_t y1,RGB16_t color);
void drawHorzLine(uint32_t y, uint32_t x0, uint32_t x1,RGB16_t color);
void drawLine(int32_t x0,int32_t y0,int32_t x1,int32_t y1,RGB16_t color);
void drawAntiAliasLine(uint32_t x0,uint32_t y0,uint32_t x1,uint32_t y1,RGB16_t color);

#endif /* PIXELBUFFER_H_ */
