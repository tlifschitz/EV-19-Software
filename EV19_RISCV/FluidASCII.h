#ifndef FLUIDASCII_H_
#define FLUIDASCII_H_

#include "HAL/EV19_Peripherals.h"
#include "fixmath.h"

#define CONSOLE_WIDTH 	CHAR_BUFFER_WIDTH
#define CONSOLE_HEIGHT 	CHAR_BUFFER_HEIGHT
#define BUFFER_SIZE 	(CONSOLE_WIDTH*CONSOLE_HEIGHT+1)
#define FPS 25

#define FRAME_DELAY_MS (1000.0/FPS)
#define USE_FLOAT 1

typedef struct
{
#if USE_FLOAT==0
	fix16_t xPos;
	fix16_t yPos;
	fix16_t xVelocity;
	fix16_t yVelocity;
	fix16_t xForce;
	fix16_t yForce;
	fix16_t density;
	int32_t wallflag;
#elif USE_FLOAT==1
	double xPos;
	double yPos;
	double xVelocity;
	double yVelocity;
	double xForce;
	double yForce;
	double density;
	int32_t wallflag;
#endif
} Particle;


typedef char Frame[BUFFER_SIZE];

typedef struct
{
	char name[15];
	Frame input;
	Frame * frames;
	uint32_t nFrames;
}Animation;


void readInput(Particle * particles, Frame * frame);

void computeFrame(Particle * particles, char* screenBuffer);

void drawFrame(char * screenBuffer);

void runSimulation();

#endif /* FLUIDASCII_H_ */
