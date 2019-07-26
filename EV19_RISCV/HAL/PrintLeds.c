#include "PrintLeds.h"
#include "EV19_Peripherals.h"


void printLeds8(int8_t n)
{
	*LEDS = n;
	while(*BUTTON == 1);
	while(*BUTTON == 0);
}

void printLeds16(int16_t n)
{
	*LEDS = (uint8_t)(n>>8);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)n;
	while(*BUTTON == 1);
	while(*BUTTON == 0);
}

void printLeds32(int32_t n)
{
	*LEDS = (uint8_t)(n>>24);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>16);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>8);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>0);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
}

void printLeds64(int64_t n)
{
	*LEDS = (uint8_t)(n>>32);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>24);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>16);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>8);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
	*LEDS = (uint8_t)(n>>0);
	while(*BUTTON == 1);
	while(*BUTTON == 0);
}

