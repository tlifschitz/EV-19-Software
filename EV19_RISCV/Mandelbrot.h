#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////
//							Macro Definition						//
//////////////////////////////////////////////////////////////////////

#define FIXED_PRECITION 	0
#define HALF_PRECITION 		1
#define SINGLE_PRECITION 	2

#define PRECITION 0

#if PRECITION == FIXED_PRECITION


	#include "fixmath.h"
#elif PRECITION == HALF_PRECITION || PRECITION == SINGLE_PRECITION
	#include <complex.h>
#endif

#if PRECITION == FIXED_PRECITION
typedef struct
{
	fix16_t real;
	fix16_t imag;
}FixedComplex;
#endif

#if PRECITION == FIXED_PRECITION
	typedef fix16_t RealNum;
	typedef FixedComplex ComplexNum;
#elif PRECITION == HALF_PRECITION
	typedef float RealNum;
	typedef float complex ComplexNum;
#elif PRECITION == SINGLE_PRECITION
	typedef double RealNum;
	typedef double complex ComplexNum;
#endif

///////////////////////////////////////////////////////////////////////

#define CURSOR_SIZE 	10
#define CURSOR_COLOR 	RGB16(255,0,0)
#define SQUARE_COLOR 	RGB16(255,255,255)
#define CURSOR_STEP 	2
#define SWIPE_STEP 		30
#define MAX_ITER		64


typedef struct
{
	int32_t x;
	int32_t y;
}Coord;

typedef enum {UP='w',DOWN='s',LEFT='a',RIGHT='d'} Direction;

/**
	Main application
*/
void interactiveMandelbrot();

/**

*/
void showCursor();

/**

*/
void hideCursor();

void toggleCursor();

/**

*/
bool moveCursor(Direction dir);

/**

*/
void swipe(Direction dir);

/**

*/
void zoom(RealNum zoomFactor);


/**

*/
void regionZoom(Coord p1, Coord p2);


#endif /* MANDELBROT_H_ */
