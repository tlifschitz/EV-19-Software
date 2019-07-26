#include "Mandelbrot.h"
#include "HAL/EV19_Peripherals.h"
#include "HAL/JTAG_UART.h"
#include "HAL/PixelBuffer.h"
#include "HAL/Timer.h"
#include <ctype.h> 	// tolower()
#include <string.h> // strcmp()
#include <stdlib.h> // atoi

#define NON_BLOCKING_COMPUTING 1

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//////////////////////////////////////////////////////////////////////
//					Typedef and Structure Definitons				//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//							Static Variables						//
//////////////////////////////////////////////////////////////////////
static RealNum x0;
static RealNum y0;
static RealNum x1;
static RealNum y1;
static RealNum realStep;
static RealNum imagStep;
static RealNum zoomFactor;
static Coord cursorPos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
// Point to zoom in region;
bool regionSelectionOn = false;
static Coord p1;
static RGB16_t savedPixels[CURSOR_SIZE*4+1+1];
static uint32_t maxIterations = MAX_ITER;
static int32_t realAxisRow = SCREEN_HEIGHT/2;
#define N_COLORS 16
const static RGB16_t colors[N_COLORS] = {	RGB16( 66,  30,  15), \
											RGB16( 25,   7,  26), \
											RGB16(  9,   1,  47), \
											RGB16(  4,   4,  73), \
											RGB16(  0,   7, 100), \
											RGB16( 12,  44, 138), \
											RGB16( 24,  82, 177), \
											RGB16( 57, 125, 209), \
											RGB16(134, 181, 229), \
											RGB16(211, 236, 248), \
											RGB16(241, 233, 191), \
											RGB16(248, 201,  95), \
											RGB16(255, 170,   0), \
											RGB16(204, 128,   0), \
											RGB16(153,  87,   0), \
											RGB16(106,  52,   3) };

//		RGB16(8,7,2), \
//											RGB16(3,2,3), \
//											RGB16(1,0,6), \
//											RGB16(0,1,9), \
//											RGB16(0,2,12), \
//											RGB16(1,11,17), \
//											RGB16(3,20,22), \
//											RGB16(7,31,25), \
//											RGB16(16,45,28), \
//											RGB16(26,58,30), \
//											RGB16(29,58,23), \
//											RGB16(30,50,12), \
//											RGB16(31,42,0), \
//											RGB16(25,32,0), \
//											RGB16(19,21,0), \
//											RGB16(13,13,0)};

//////////////////////////////////////////////////////////////////////
//   				    Static Function Prototipes					//
//////////////////////////////////////////////////////////////////////
/**

*/
static void computeScreen(uint32_t x, uint32_t y, uint32_t width, uint32_t height,bool simetryOptimization);

static void defaultParameters();
/**

*/
static void getConsoleInput();

/**

*/
static ComplexNum pixelToComplex(Coord coord);
//
///**
//
//*/
//static Coord complexToPixel(ComplexNum complex);

/**

*/
//static void iterateOverCursor(RGB16_t (*color)(uint32_t x, uint32_t y));


/**

*/
static RGB16_t computePixel(uint32_t x, uint32_t y);

/**

*/
static RGB16_t cursorColor(uint32_t x, uint32_t y);

/**
 *
 */
static uint32_t iterationCount(RealNum cReal, RealNum cImag);

static RGB16_t collorPallete(uint32_t iterations);

//////////////////////////////////////////////////////////////////////
//   				        Function Definition  					//
//////////////////////////////////////////////////////////////////////

typedef enum {IDLE, RENDERING, ZOOMING_REGION} Status_t;

void interactiveMandelbrot()
{
	UART_Write("------------- Interactive Mandelbrot -------------\n");

	fillScreen(RGB16(0,0,0));

//	for(int i=0; i<CURSOR_SIZE*4+1+1; i++)
//		savedPixels[i] = CURSOR_COLOR;


	bool appRunning = true;
	char c;
	
	defaultParameters();

	char num[20];
//	UART_Write("\nX0: ");
//	fix16_to_string(x0,num);
//	UART_Write(num);
//	UART_Write("\nY0: ");
//	fix16_to_string(y0,num);
//	UART_Write(num);
//	UART_Write("\nX1: ");
//	fix16_to_string(x1,num);
//	UART_Write(num);
//	UART_Write("\nY1: ");
//	fix16_to_string(y1,num);
//	UART_Write(num);
//	UART_Write("\nRealStep: ");
//	fix16_to_string(realStep,num);
//	UART_Write(num);
//	UART_Write("\nImagStep: ");
//	fix16_to_string(imagStep,num);
//	UART_Write(num);

	UART_Write("Computing\n");

	computeScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,true);

	showCursor();

	while(appRunning)
	{
		if(UART_ReadCharNB(&c))
		{
			if(isalnum(c))
				c = tolower(c);

			UART_WriteChar(c);

			switch(c)
			{
				case 'w': case 'a': case 's': case 'd':
					if(moveCursor(c)==false)
						swipe(c);
				break;

				case 'z': // Zoom in
					zoom(zoomFactor);
				break;

				case 'x': // Zoom out
#if PRECITION == FIXED_PRECITION
				zoom(fix16_div(fix16_one,zoomFactor));
#else
				zoom(1/zoomFactor);
#endif
				break;

				case 'c': // Console
					getConsoleInput();
				break;

				case 'r': // Reset
					defaultParameters();
					computeScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,true);
					showCursor();
				break;

				case 'e': // Zoom in region
					if(regionSelectionOn == false)
					{
						UART_Write("ZOOM IN REGION!\n");
						p1 = cursorPos;
					}
					regionSelectionOn = !regionSelectionOn;
				break;

				case '\r': // Enter
					if(regionSelectionOn == true)
					{
						UART_Write("ZOOMING!\n");
						regionSelectionOn = false;
						regionZoom(p1,cursorPos);
					}
				break;
			}
		}
	}
}


void iterateOverCursor(RGB16_t (*colorFun)(uint32_t x, uint32_t y))
{
	for(int32_t x = MAX(0,cursorPos.x - CURSOR_SIZE); x < MIN(SCREEN_WIDTH,cursorPos.x + CURSOR_SIZE); x++)
		PIXEL_BUFFER[cursorPos.y][x] = colorFun(x,cursorPos.y);

	for(int32_t y = MAX(0,cursorPos.y - CURSOR_SIZE); y < MIN(SCREEN_HEIGHT,cursorPos.y + CURSOR_SIZE); y++)
		PIXEL_BUFFER[y][cursorPos.x] = colorFun(cursorPos.x,y);
}

void hideCursor()
{
	uint32_t p0x = MIN(cursorPos.x,p1.x);
	uint32_t p0y = MAX(cursorPos.y,p1.y);
	uint32_t p1x = MAX(cursorPos.x,p1.x);
	uint32_t p1y = MIN(cursorPos.y,p1.y);
	uint32_t width = abs(p1x-p0x);
	uint32_t height =abs(p1y-p0y);

	if(regionSelectionOn == true)
	{
		computeScreen(p0x,p0y,width,1,false);
		computeScreen(p0x,p0y,1,height,false);
		computeScreen(p0x+width,p0y,1,height,false);
		computeScreen(p0x,p0y+height,width,1,false);
	}

	computeScreen(cursorPos.x-CURSOR_SIZE,cursorPos.y,2*CURSOR_SIZE+1,1,false);
	computeScreen(cursorPos.x,cursorPos.y-CURSOR_SIZE,1,2*CURSOR_SIZE+1,false);
	return;
#if PRECITION==FIXED_PRECITION
	RealNum imag = y0 + fix16_mul(cursorPos.y*fix16_one,imagStep);
	RealNum real = x0 + fix16_mul(MAX(0,cursorPos.x - CURSOR_SIZE)*fix16_one,realStep);
#else
	RealNum imag = y0 + cursorPos.y*imagStep;
	RealNum real = x0 + MAX(0,cursorPos.x - CURSOR_SIZE)*realStep;
#endif

	for(int32_t x = MAX(0,cursorPos.x - CURSOR_SIZE); x < MIN(SCREEN_WIDTH,cursorPos.x + CURSOR_SIZE); x++)
	{
			PIXEL_BUFFER[cursorPos.y][x] = collorPallete(iterationCount(real,imag));//savedPixels[i];
			real += realStep;
			delay(9);
	}
#if PRECITION==FIXED_PRECITION
	imag = y0 + fix16_mul(MAX(0,cursorPos.y - CURSOR_SIZE)*fix16_one,imagStep);
	real = x0 + fix16_mul(cursorPos.x*fix16_one,realStep);
#else
	imag = y0 + MAX(0,cursorPos.y - CURSOR_SIZE)*imagStep;
	real = x0 + cursorPos.x*realStep;
#endif

	for(int32_t y = MAX(0,cursorPos.y - CURSOR_SIZE); y < MIN(SCREEN_HEIGHT,cursorPos.y + CURSOR_SIZE); y++)
	{
			PIXEL_BUFFER[y][cursorPos.x] = collorPallete(iterationCount(real,imag));//savedPixels[i];
			imag += imagStep;
			delay(9);
	}
}

void showCursor()
{
	uint32_t p0x = MIN(cursorPos.x,p1.x);
	uint32_t p0y = MIN(cursorPos.y,p1.y);
	uint32_t p1x = MAX(cursorPos.x,p1.x);
	uint32_t p1y = MAX(cursorPos.y,p1.y);

	if(regionSelectionOn == true)
	{
		drawHorzLine(p0y,p0x,p1x, SQUARE_COLOR);
		drawHorzLine(p1y,p0x,p1x, SQUARE_COLOR);
		drawVertLine(p0x,p0y,p1y, SQUARE_COLOR);
		drawVertLine(p1x,p0y,p1y, SQUARE_COLOR);
	}

	drawHorzLine(cursorPos.y, MAX(0,cursorPos.x - CURSOR_SIZE), MIN(SCREEN_WIDTH,cursorPos.x + CURSOR_SIZE),CURSOR_COLOR);
	drawVertLine(cursorPos.x, MAX(0,cursorPos.y - CURSOR_SIZE), MIN(SCREEN_HEIGHT,cursorPos.y + CURSOR_SIZE),CURSOR_COLOR);



//	//int i=0;
//	for(int32_t x = MAX(0,cursorPos.x - CURSOR_SIZE); x < MIN(SCREEN_WIDTH,cursorPos.x + CURSOR_SIZE); x++)
//	{
//		//savedPixels[i] = PIXEL_BUFFER[cursorPos.y][x];
//		PIXEL_BUFFER[cursorPos.y][x] =	CURSOR_COLOR;
//		//i++;
//		delay(8);
//	}
//
//	for(int32_t y = MAX(0,cursorPos.y - CURSOR_SIZE); y < MIN(SCREEN_HEIGHT,cursorPos.y + CURSOR_SIZE); y++)
//	{
//		//savedPixels[i] = PIXEL_BUFFER[y][cursorPos.x];
//		PIXEL_BUFFER[y][cursorPos.x] = CURSOR_COLOR;
//		//i++;
//		delay(8);
//	}
}


/**

*/
void toggleCursor()
{
	int i = 0;

	for(int32_t x = MAX(0,cursorPos.x - CURSOR_SIZE); x < MIN(SCREEN_WIDTH,cursorPos.x + CURSOR_SIZE); x++)
	{
		RGB16_t temp = PIXEL_BUFFER[cursorPos.y][x];
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		savedPixels[i] = temp;
		i++;
		temp = savedPixels[i];
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		PIXEL_BUFFER[cursorPos.y][x] = temp;
	}

	for(int32_t y = MAX(0,cursorPos.y - CURSOR_SIZE); y < MIN(SCREEN_HEIGHT,cursorPos.y + CURSOR_SIZE); y++)
	{
		RGB16_t temp = PIXEL_BUFFER[y][cursorPos.x];

		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		savedPixels[i] = temp;
		i++;
		temp = savedPixels[i];
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		PIXEL_BUFFER[y][cursorPos.x] = temp;
	}

}

/**

*/
bool moveCursor(Direction dir)
{
	bool retval = true;

	hideCursor();
	switch(dir)
	{
		case UP:
			if(cursorPos.y >= CURSOR_STEP)
				cursorPos.y -= CURSOR_STEP;
			else retval = false;
		break;

		case DOWN:
			if(cursorPos.y + CURSOR_STEP < SCREEN_HEIGHT)
				cursorPos.y += CURSOR_STEP;
			else retval = false;
		break;

		case LEFT:
			if(cursorPos.x >= CURSOR_STEP)
				cursorPos.x -= CURSOR_STEP;
			else retval = false;
		break;

		case RIGHT:
			if(cursorPos.x + CURSOR_STEP < SCREEN_WIDTH)
				cursorPos.x += CURSOR_STEP;
			else retval = false;
		break;
	}

	showCursor();

	return retval;
}

/**

*/
void swipe(Direction dir)
{
	int i,j;
	hideCursor();
	switch(dir)
	{
		case DOWN:
#if PRECITION == FIXED_PRECITION
			y0 += fix16_mul(SWIPE_STEP*fix16_one,imagStep); // imagStep es negativo!!
			y1 += fix16_mul(SWIPE_STEP*fix16_one,imagStep);
#else
			y0 += SWIPE_STEP*imagStep; // imagStep es negativo!!
			y1 += SWIPE_STEP*imagStep;
#endif
			realAxisRow += SWIPE_STEP;
			i=0;
			for(; i<SCREEN_HEIGHT-SWIPE_STEP; i++)
			{
				for(int j=0; j<SCREEN_WIDTH; j++)
				{
					PIXEL_BUFFER[i][j] = PIXEL_BUFFER[i+SWIPE_STEP][j];
					delay(12);
				}
			}

//			for(; i<SCREEN_HEIGHT; i++)
//				for(int j=0; j<SCREEN_WIDTH; j++)
//					PIXEL_BUFFER[i][j] = collorPallete(iterationCount(j,i));
			computeScreen(0,i,SCREEN_WIDTH,SWIPE_STEP,false);
		break;

		case UP:
#if PRECITION == FIXED_PRECITION
			y0 -= fix16_mul(SWIPE_STEP*fix16_one,imagStep); // imagStep es negativo!!
			y1 -= fix16_mul(SWIPE_STEP*fix16_one,imagStep);
#else
			y0 -= SWIPE_STEP*imagStep; // imagStep es negativo!!
			y1 -= SWIPE_STEP*imagStep;
#endif

			realAxisRow -= SWIPE_STEP;
			i = SCREEN_HEIGHT-1;
			for(; i>=SWIPE_STEP; i--)
			{
				for(int j=0; j<SCREEN_WIDTH; j++)
				{
					PIXEL_BUFFER[i][j] = PIXEL_BUFFER[i-SWIPE_STEP][j];
					delay(12);
				}
			}
//			for(; i>=0; i--)
//				for(int j=0; j<SCREEN_WIDTH; j++)
//					PIXEL_BUFFER[i][j] = collorPallete(iterationCount(j,i));
			computeScreen(0,0,SCREEN_WIDTH,SWIPE_STEP,false);

		break;

		case RIGHT:
#if PRECITION == FIXED_PRECITION
			x0 += fix16_mul(SWIPE_STEP*fix16_one,realStep);
			x1 += fix16_mul(SWIPE_STEP*fix16_one,realStep);
#else
			x0 += SWIPE_STEP*realStep;
			x1 += SWIPE_STEP*realStep;
#endif

			j=0;
			for(; j<SCREEN_WIDTH-SWIPE_STEP; j++)
			{
				for(int i=0; i<SCREEN_HEIGHT; i++)
				{
					PIXEL_BUFFER[i][j] = PIXEL_BUFFER[i][j+SWIPE_STEP];
					delay(12);
				}
			}
//			for(; j<SCREEN_WIDTH; j++)
//				for(int i=0; i<SCREEN_HEIGHT; i++)
//					PIXEL_BUFFER[i][j] = collorPallete(iterationCount(j,i));

			computeScreen(SCREEN_WIDTH-SWIPE_STEP,0,SWIPE_STEP,SCREEN_HEIGHT,false);
		break;

		case LEFT:
#if PRECITION == FIXED_PRECITION
			x0 -= fix16_mul(SWIPE_STEP*fix16_one,realStep);
			x1 -= fix16_mul(SWIPE_STEP*fix16_one,realStep);
#else
			x0 -= SWIPE_STEP*realStep;
			x1 -= SWIPE_STEP*realStep;
#endif


			j=SCREEN_WIDTH-1;
			for(; j>=SWIPE_STEP; j--)
			{
				for(int i=0; i<SCREEN_HEIGHT; i++)
				{
					PIXEL_BUFFER[i][j] = PIXEL_BUFFER[i][j-SWIPE_STEP];
					delay(12);
				}
			}
//			for(; j>=0; i--)
//				for(int i=0; i<SCREEN_HEIGHT; i++)
//					PIXEL_BUFFER[i][j] = collorPallete(iterationCount(j,i));
			computeScreen(0,0,SWIPE_STEP,SCREEN_HEIGHT,false);
		break;
	}
	showCursor();

}

/**

*/
void zoom(RealNum factor)
{
#if PRECITION == FIXED_PRECITION
	x0 += fix16_mul(fix16_mul(cursorPos.x*fix16_one, realStep),fix16_one-factor);
	y0 += fix16_mul(fix16_mul(cursorPos.y*fix16_one, imagStep),fix16_one-factor);
	realStep = fix16_mul(realStep, factor);
	imagStep = fix16_mul(imagStep, factor);
	realAxisRow = fix16_to_int(fix16_div(-y0,imagStep));

#else
	x0 += cursorPos.x * realStep * (factor -1);
	y0 += cursorPos.y * imagStep * (factor -1);
	realStep = realStep / factor;
	imagStep = imagStep / factor;
	realAxisRow = (int32_t)(-y0/imagStep);
#endif

	computeScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,true);
	showCursor();
}



/**

*/
void regionZoom(Coord p1, Coord p2)
{
	uint32_t p1x = MIN(p1.x,p2.x);
	uint32_t p1y = MAX(p1.y,p2.y);

	uint32_t p2x = MAX(p1.x,p2.x);
	uint32_t p2y = MIN(p1.y,p2.y);

	RealNum x0temp = x0;
	RealNum y0temp = y0;

#if PRECITION==FIXED_PRECITION
	x0 = x0temp + fix16_mul(realStep,fix16_one*p1x);
	y0 = y0temp + fix16_mul(imagStep,fix16_one*p1y);
	x1 = x0temp + fix16_mul(realStep,fix16_one*p2x);
	y1 = y0temp + fix16_mul(imagStep,fix16_one*p2y);
	realStep = fix16_div((x1-x0),SCREEN_WIDTH*fix16_one);
	imagStep = fix16_div((y1-y0),SCREEN_HEIGHT*fix16_one);
	realAxisRow = fix16_to_int(fix16_div(-y0,imagStep));
#else
	x0 = x0temp + realStep*p1x;
	y0 = y0temp + imagStep*p1y;
	x1 = x0temp + realStep*p2x;
	y1 = y0temp + imagStep*p2y;
	realStep = (x1-x0)/SCREEN_WIDTH;
	imagStep = (y1-y0)/SCREEN_HEIGHT;
	realAxisRow = (int32_t)(-y0/imagStep);
#endif

	cursorPos.x = SCREEN_WIDTH/2;
	cursorPos.y = SCREEN_HEIGHT/2;

	computeScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,true);
	showCursor();
}




/**

*/
static void computeScreen(uint32_t x, uint32_t y, uint32_t width, uint32_t height,bool simetryOptimization)
{
#if PRECITION==FIXED_PRECITION
	RealNum cImag = y0 + fix16_mul(y*fix16_one,imagStep);
#else
	RealNum cImag = y0 + y*imagStep;
#endif

	//uint32_t nPixels = width*height;

	for(int i = y; i < y + height; i++)
	{

		// Copy mirrored part
		if(simetryOptimization==true && realAxisRow<i && i<=2*realAxisRow)
		{
			for (int j = x; j < x + width; j++)
			{
				//UART_Write("COPY\n");
				//LED_PROGRESS_BAR(i*width+j,nPixels);
				PIXEL_BUFFER[i][j] = PIXEL_BUFFER[2*realAxisRow-i][j];
				delay(9);
			}
		}
		else
		{
#if PRECITION==FIXED_PRECITION
			RealNum cReal = x0 + fix16_mul(x*fix16_one,realStep);
#else
			RealNum cReal = x0 + x*realStep;
#endif

			for (int j = x; j < x + width; j++)
			{

				//LED_PROGRESS_BAR(i*width+j,nPixels);
				RealNum zRealSq = 0;
				RealNum zImagSq = 0;
				RealNum zsquare = 0;
				int iter = maxIterations;
#if PRECITION==FIXED_PRECITION
				while (zRealSq + zImagSq <= 100*fix16_one  && iter--)
#else
				while (zRealSq + zImagSq <= 4  && iter--)
#endif
				{

					RealNum x = zRealSq - zImagSq + cReal;
					RealNum y = zsquare - zRealSq - zImagSq + cImag;

					#if PRECITION==FIXED_PRECITION
					zRealSq = fix16_mul(x,x);
					zImagSq = fix16_mul(y,y);
					zsquare = fix16_mul(x + y,x + y);
					#else
					zRealSq = x*x;
					zImagSq = y*y;
					zsquare = (x + y)*(x + y);
					#endif
				}

				RGB16_t col = colors[(maxIterations-iter)%N_COLORS];
				asm("nop");
				PIXEL_BUFFER[i][j] = col;

#if PRECITION==FIXED_PRECITION
				cReal = fix16_add(cReal, realStep);
#else
				cReal += realStep;
#endif

			}
		}

#if PRECITION==FIXED_PRECITION
				cImag = fix16_add(cImag, imagStep);
#else
				cImag += imagStep;
#endif
	}
}

/**
 *
 */
static void defaultParameters()
{
	realAxisRow = SCREEN_HEIGHT/2;

#if PRECITION == FIXED_PRECITION
	x0 = fix16_from_dbl(-2);
	y0 = fix16_from_dbl(1.5);
	x1 = fix16_from_dbl(1);
	y1 = fix16_from_dbl(-1.5);
	zoomFactor = fix16_from_dbl(0.5);
	realStep = fix16_div((x1-x0),SCREEN_WIDTH*fix16_one);
	imagStep = fix16_div((y1-y0),SCREEN_HEIGHT*fix16_one);
#else
	x0 = -2;
	y0 = 1.5;
	x1 = 1;
	y1 = -1.5;
	zoomFactor = 2;
	realStep = (x1-x0)/SCREEN_WIDTH;
	imagStep = (y1-y0)/SCREEN_HEIGHT;
#endif
}


/**

*/
static void getConsoleInput()
{
	char input[50];
	uint8_t i = 0;
	while( ( input[i++] = UART_ReadChar() ) != '\r');
	i--;
	input[i] = '\0';

	char * command = strtok(input, "=");

	if(strcmp(command,"iterations")==0)
	{
		UART_Write("ITERATIONS = ");
		char * num = strtok(NULL, "=");
		if(num!=NULL)
		{
			UART_Write(num);
			maxIterations = atoi(num);
			computeScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,true);
			showCursor();
		}
	}

//	else if()
}

/**

*/
static ComplexNum pixelToComplex(Coord coord)
{
	ComplexNum c;
#if PRECITION == FIXED_PRECITION
	c.real = 0;
	c.imag = 0;
#elif PRECITION == HALF_PRECITION || PRECITION == SINGLE_PRECITION
	c = x0 + y1*I + realStep * coord.x + imagStep * coord.y * I;
#endif
	return c;
}
//
///**
//
//*/
//static Coord complexToPixel(ComplexNum complex)
//{
//	Coord c;
//#if PRECITION == FIXED_PRECITION
//	c.x = fix16_to_int(fix16_div(fix16_sub(complex.real, x0), realStep));
//	c.y = fix16_to_int(fix16_div(fix16_sub(complex.imag, y1), imagStep));
//#elif PRECITION == HALF_PRECITION
//	c.x = crealf(complex - upperLeft) / realStep;
//	c.y = crealf(complex - upperLeft) / imagStep;
//#elif PRECITION == SINGLE_PRECITION
//	c.x = creal(complex - upperLeft) / realStep;
//	c.y = creal(complex - upperLeft) / imagStep;
//#endif
//	return c;
//}

/**

*/
static RGB16_t computePixel(uint32_t x, uint32_t y)
{
	uint32_t n = iterationCount(x,y);
	volatile RGB16_t c = collorPallete(n);
	*LEDS = c;
	return c;

}

/**

*/
static RGB16_t cursorColor(uint32_t x, uint32_t y)
{
	return CURSOR_COLOR;
}

static uint32_t iterationCount(RealNum cReal, RealNum cImag)
{
	RealNum zRealSq = 0;
	RealNum zImagSq = 0;
	RealNum zsquare = 0;
	RealNum x = 0;
	RealNum y = 0;
	int i = maxIterations;
#if PRECITION==FIXED_PRECITION
	while (zRealSq + zImagSq <= 4*fix16_one  && i--)
#else
	while (zRealSq + zImagSq <= 4  && i--)
#endif
	{
		x = zRealSq - zImagSq + cReal;
		y = zsquare - zRealSq - zImagSq + cImag;

#if PRECITION==FIXED_PRECITION
		zRealSq = fix16_mul(x,x);
		zImagSq = fix16_mul(y,y);
		zsquare = fix16_mul(x + y,x + y);
#else
		zRealSq = x*x;
		zImagSq = y*y;
		zsquare = (x + y)*(x + y);
#endif

	}
	return maxIterations-i;
}

static inline RGB16_t collorPallete(uint32_t iterations)
{
	return colors[iterations % N_COLORS];
}
