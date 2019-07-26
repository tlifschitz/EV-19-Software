#include "Scope.h"
#include "HAL/EV19_Peripherals.h"
#include "HAL/JTAG_UART.h"
#include "HAL/PixelBuffer.h"
//#include "HAL/Timer.h"
#include <string.h>

typedef enum {SAMPLING, RENDER} State_t;

#define BUFF_SIZE (2*SCREEN_WIDTH)
#define SAMPLE_TO_PIXEL(s) ((SCREEN_HEIGHT-1) - (((SCREEN_HEIGHT-1) * s) / 0xFFF ))

//static int16_t samples[SCREEN_WIDTH];
static int32_t sampleIndex = 0;
static int16_t screenIndex = 0;
static int16_t samples[BUFF_SIZE];
static int16_t screenSamples[SCREEN_WIDTH];
static int16_t triggerLevel;
static int16_t horizontalPosition;
static int16_t triggerIndex;
static bool triggered;

void setTriggerLevel(int16_t level)
{
	triggerLevel = level;
}

void setHorizontalPosition(int16_t pos)
{
	horizontalPosition = pos;
}

void drawGrid()
{
	drawVertLine(1*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(2*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(3*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(4*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(5*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(6*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(7*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(8*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(9*64,0,20,GRID_COLOR);
	delay(50);
	drawVertLine(10*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+0*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+1*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+2*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+3*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+4*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+5*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+6*64,0,20,GRID_COLOR);
	delay(50);
	drawHorzLine(16+7*64,0,20,GRID_COLOR);
	delay(50);
}

void ScopeMain()
{
	UART_Write("SCOPE!\n");

	// Draw background
	fillScreen(BACKGROUND_COLOR);

	// Set trigger and horizontal in the middle of the screen
	setTriggerLevel(2048);
	setHorizontalPosition(0);
	triggered = false;

//	drawGrid();

	// Clear buffers
	memset((void*)samples,255,BUFF_SIZE*sizeof(int16_t));
	memset((void*)screenSamples,0,SCREEN_WIDTH*sizeof(int16_t));

	// Start ADC auto sampling
	ADC[1] = 1;

	int16_t samplesAfterTrigger=0;
	State_t state = SAMPLING;
	while(1)
	{
//		char c;
//		if(UART_ReadCharNB(&c))
//		{
//
//			switch(c)
//			{
//				case 'w':
//					drawLine(0,SAMPLE_TO_PIXEL(triggerLevel),SCREEN_WIDTH,SAMPLE_TO_PIXEL(triggerLevel),BACKGROUND_COLOR);
//					triggerLevel+=5;
//				break;
//
//				case 's':
//					drawLine(0,SAMPLE_TO_PIXEL(triggerLevel),SCREEN_WIDTH,SAMPLE_TO_PIXEL(triggerLevel),BACKGROUND_COLOR);
//					triggerLevel-=5;
//				break;
//
//				case 'a':
//					horizontalPosition-=5;
//				break;
//
//				case 'd':
//					horizontalPosition+=5;;
//				break;
//			}
//		}
		switch(state)
		{
		case SAMPLING:

			samplesAfterTrigger = 0;

			while(horizontalPosition + samplesAfterTrigger < SCREEN_WIDTH)
			{

				// Store the sample
				samples[sampleIndex] = ADC[0]&0x00000FFF;

				// Display it with leds
				//*LEDS = samples[sampleIndex];

				// Check trigger condition
				if(!triggered && samples[(sampleIndex+BUFF_SIZE-1)%BUFF_SIZE]<triggerLevel && samples[sampleIndex]>=triggerLevel)
				{
		//			UART_Write("Triggered\n");
					triggerIndex = sampleIndex;
					triggered = true;
				}
				else if(triggered)
					samplesAfterTrigger++;

				sampleIndex = (sampleIndex+1)%BUFF_SIZE;

				delay(100);
			}

			//UART_Write("Rendering\n");
			triggered = false;
			state = RENDER;

			break;

		case RENDER:

			// Start -horizontalPosition- samples previous to trigger
			sampleIndex = (triggerIndex+BUFF_SIZE-horizontalPosition)%BUFF_SIZE;
			screenIndex = 1;
			int16_t prevSample;
			int16_t curSample =  SAMPLE_TO_PIXEL(samples[sampleIndex]);

			while(screenIndex < SCREEN_WIDTH)
			{
				// Delete previous line
				drawLine(screenIndex-1,screenSamples[screenIndex-1],screenIndex,screenSamples[screenIndex],BACKGROUND_COLOR);

				// Compute next sample
				prevSample = curSample;
				sampleIndex = (sampleIndex + 1) % BUFF_SIZE;
				curSample  = SAMPLE_TO_PIXEL(samples[sampleIndex]);

				// Draw new line
				drawLine(screenIndex-1,prevSample,screenIndex,curSample,SIGNAL_COLOR);

				// Store sample
				screenSamples[screenIndex-1] = prevSample;

				screenIndex++;

			}


			drawGrid();
			drawLine(0,SAMPLE_TO_PIXEL(triggerLevel),SCREEN_WIDTH,SAMPLE_TO_PIXEL(triggerLevel),RGB16(200,200,200));
			//UART_Write("Sampling\n");

			state = SAMPLING;
			delay(40000);
			break;
		}
	}
}


