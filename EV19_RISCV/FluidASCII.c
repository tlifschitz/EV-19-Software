#include "FluidASCII.h"
#include <math.h>
#include "HAL/Timer.h"

#ifdef Win32
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>


//#define OUTPUT_TO_FILE 1

#define END_OF_FILE '\0'

#define LED_PROGRESS_BAR(n,N)

#ifdef _WIN32
// Some old MinGW/CYGWIN distributions don't define this:
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif
#endif

#else
#include "HAL/EV19_Peripherals.h"
#include "HAL/JTAG_UART.h"
#include "malloc.h"

#define END_OF_FILE '\t'

#endif

static int totalOfParticles;

#ifdef Win32
static FILE *output;
#endif

#if USE_FLOAT==0
#define gravity  (1*fix16_one)
#define pressure  (4*fix16_one)
#define viscosity  (7*fix16_one)
#elif USE_FLOAT==1
#define gravity  (1.0)
#define pressure  (4.0)
#define viscosity  (7.0)
#endif


static void clearScreen()
{
#ifdef Win32
	UART_Write("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	//UART_Write("\x1b[2J");
#else
	for(int i=0; i<CHAR_BUFFER_HEIGHT; i++)
		for(int j=0; j<CHAR_BUFFER_WIDTH; j++)
			PUTCHAR(j,i,0);
#endif
}


void runSimulation()
{

#ifdef OUTPUT_TO_FILE
#if USE_FLOAT==0
	output = fopen("outputFIXED.txt", "w");
#elif USE_FLOAT==1
	output = fopen("outputFLOAT.txt", "w");
#endif

	if (output == NULL)
	{
		printf("Unable to create file.\n");
		exit(EXIT_FAILURE);
	}
#endif

	clearScreen();

	// Init memory block
	malloc_addblock((void*)(SDRAM_BASE),0x2000000);

	// Allocate memory for particles
	Particle * particles = (Particle*)malloc(CONSOLE_WIDTH * CONSOLE_HEIGHT * 2 * sizeof(Particle));
	if(particles==NULL)
	{
		UART_Write("ERROR ALLOCATING MEMORY! (particles)\n");
		while(1);
	}
	memset((void*)particles, 0, CONSOLE_WIDTH * CONSOLE_HEIGHT * 2 * sizeof(Particle));

	// Allocate memory for one animation.
	Animation * animation = malloc(sizeof(Animation));
	if(animation==NULL)
	{
		UART_Write("ERROR ALLOCATING MEMORY (animation)!\n");
		while(1);
	}

	// Read input from stdin, store it inside the animation and init particles.
	readInput(particles,&animation->input);
	UART_Write("HOLA\n");

	const uint8_t durationSecs = 1;
	const uint8_t nFrames = durationSecs*FPS;

	// Allocate memory for frames
	animation->frames = (Frame*)malloc(nFrames * sizeof(Frame));
	UART_Write("HOLAAAA\n");

	if(animation->frames==NULL)
	{
		UART_Write("ERROR ALLOCATING MEMORY (frames)!\n");
	}
	else
	{
		UART_Write("ALLOCATION (frames) successfull!\n");
	}
	memset((void*)animation->frames, 0,nFrames * sizeof(Frame));

#ifdef OUTPUT_TO_FILE
	fprintf(output, "\n\n\n\n");
#endif

	for (int i = 0; i < nFrames; i++)
	{

		LED_PROGRESS_BAR(i, nFrames);
		char progress[4] = "000";
		itoa(i * 100 / nFrames, progress, 10);
		UART_Write("\rRendering: ");
		UART_Write(progress);
		UART_Write("%");

#ifdef OUTPUT_TO_FILE
		fprintf(output,"--------------------- FRAME %d ---------------------\n\n", i);
#endif
		computeFrame(particles,animation->frames[i]);
#ifdef OUTPUT_TO_FILE
		fprintf(output, "---------------------------------------------------\n\n");
#endif
	}


#ifdef OUTPUT_TO_FILE
	fclose(output);
#endif




play:
	clearScreen();
	for (int i = 0; i < nFrames; i++)
	{
		LED_PROGRESS_BAR(i, nFrames);
		drawFrame(animation->frames[i]);
		delay(1000);
//		Sleep(FRAME_DELAY_MS);
	}
	while(*BUTTON==1);
	goto play;

}
void readInput(Particle * particles, Frame * frame)
{
	// read the input file to initialise the particles.
	// # stands for "wall", i.e. unmovable particles (very high density)
	// any other non-space character represents normal particles.

	char c = 0;
	int particlesCounter = 0;
	int xSandboxAreaScan = 0;
	int ySandboxAreaScan = 0;

	UART_Write("WAITING FOR INPUT\n");
	while ((c = UART_ReadChar()) != END_OF_FILE)
	{
		if(c=='\r') c = '\n';

		UART_WriteChar(c);
		
		switch ( c )
		{
			case '\n':
				// next row
				// rewind the x to -1 cause it's gonna be incremented at the
				// end of the while body
				ySandboxAreaScan += 2;
				xSandboxAreaScan = -1;
				break;
			case ' ':
				break;
			case '#':
				// The character # represents “wall particle” (a particle with fixed position),
				// and any other non-space characters represent free particles.
				// A wall sets the flag on 2 particles side by side.
				particles[particlesCounter].wallflag = 1;
				particles[particlesCounter + 1].wallflag = 1;
			default:
				// Each non-empty character sets the position of two
				// particles one below the other (real part is rows)
				// i.e. each cell in the input file corresponds to 1x2 particle spaces,
				// and each character sets two particles
				// one on top of each other.
				// It's as if the input map maps to a space that has twice the height, as if the vertical
				// resolution was higher than the horizontal one.
				// This is corrected later, see "y scale correction" comment.
				// I think this is because of gravity simulation, the vertical resolution has to be
				// higher, or conversely you can get away with simulating a lot less of what goes on in the
				// horizontal axis.
#if USE_FLOAT==0
				particles[particlesCounter].xPos = xSandboxAreaScan*fix16_one;
				particles[particlesCounter].yPos = ySandboxAreaScan*fix16_one;

				particles[particlesCounter + 1].xPos = xSandboxAreaScan*fix16_one;
				particles[particlesCounter + 1].yPos = (ySandboxAreaScan + 1)*fix16_one;

#elif USE_FLOAT==1
				particles[particlesCounter].xPos = (double)xSandboxAreaScan;// +(double)rand() / (double)RAND_MAX - 1.0;
				particles[particlesCounter].yPos = (double)ySandboxAreaScan ;

				particles[particlesCounter + 1].xPos = (double)xSandboxAreaScan;// +(double)rand() / (double)RAND_MAX - 1.0;
				particles[particlesCounter + 1].yPos = (double)(ySandboxAreaScan + 1);
#endif
				// we just added two particles
				totalOfParticles = particlesCounter += 2;

#ifdef OUTPUT_TO_FILE
#if USE_FLOAT==0
		fprintf(output, "Particle %d: [%f,%f]\n", particlesCounter, fix16_to_dbl(particles[particlesCounter].xPos), fix16_to_dbl(particles[particlesCounter].yPos));
#elif USE_FLOAT==1
		fprintf(output, "Particle %d: [%f,%f]\n",particlesCounter, xSandboxAreaScan, ySandboxAreaScan);
#endif
#endif
		}
		// next column
		xSandboxAreaScan += 1;
	}
	UART_Write("\nFINISHED READING INPUT\n");
}

void drawFrame(char * screenBuffer)
{
#ifdef Win32
	puts(screenBuffer);
	//puts("\n\n\n\n");
#else
//	UART_Write(screenBuffer);
//	UART_Write("\n\n\n\n\n\n");
	uint8_t row = 0;
	uint8_t col = 0;
	for (int j = 0; j < 80 * 24; j++)
	{
		if (screenBuffer[j] == '\n')
		{
			row++;
			col = 0;
		}
		else
		{
			PUTCHAR(col, row, screenBuffer[j]);
			col++;
		}
	}
#endif
}

void computeFrame(Particle * particles, char* screenBuffer)
{
#if USE_FLOAT==0
	fix16_t  xParticleDistance;
	fix16_t  yParticleDistance;
	fix16_t  particlesInteraction;
	fix16_t  particlesDistance;
#elif USE_FLOAT==1
	double  xParticleDistance;
	double  yParticleDistance;
	double  particlesInteraction;
	double  particlesDistance;
#endif

	// Iterate over every pair of particles to calculate the densities
	//UART_Write("Computing densities..\n");
	for (int i = 0; i < totalOfParticles; i++)
	{
		LED_PROGRESS_BAR(i,totalOfParticles);

		// Density of "wall" particles is high, other particles will bounce off them.
#if USE_FLOAT==0
		particles[i].density = particles[i].wallflag * 20 * fix16_one;
#elif USE_FLOAT==1
		particles[i].density = particles[i].wallflag * 9 ;
#endif

		for (int j = 0; j < totalOfParticles; j++)
		{

#if USE_FLOAT==0
			xParticleDistance = fix16_sub(particles[i].xPos, particles[j].xPos);
			yParticleDistance = fix16_sub(particles[i].yPos, particles[j].yPos);

			particlesDistance = fix16_vec_mod(xParticleDistance, yParticleDistance);
			//particlesDistance = fix16_smul(xParticleDistance,xParticleDistance) + fix16_smul(yParticleDistance,yParticleDistance);
			particlesInteraction = fix16_sub(fix16_sdiv(particlesDistance, fix16_one * 2), fix16_one);

			// this line here with the alternative test
			// works much better visually but breaks simmetry with the
			// next block
			//if (round(creal(particlesInteraction)) < 1){
			// density is updated only if particles are close enough
			if (fix16_to_int(fix16_sub(fix16_one, particlesInteraction)) > 0)
				//if(particlesDistance<4*fix16_one)
			{
				particles[i].density = fix16_add(particles[i].density, fix16_square(particlesInteraction));
			}
#elif USE_FLOAT==1
			xParticleDistance = particles[i].xPos - particles[j].xPos;
			yParticleDistance = particles[i].yPos - particles[j].yPos;

			particlesDistance = sqrt( xParticleDistance*xParticleDistance + yParticleDistance*yParticleDistance );
			
			particlesInteraction = particlesDistance/2.0 - 1.0;

			if (floor(1.0 - particlesInteraction) > 0)
				particles[i].density += particlesInteraction*particlesInteraction;
#endif

		}
#ifdef OUTPUT_TO_FILE
#if USE_FLOAT==0
		fprintf(output, "Density %d: %f\n", i, fix16_to_dbl(particles[i].density));
#elif USE_FLOAT==1
		fprintf(output, "Density %d: %f\n", i, particles[i].density);
#endif
#endif
	}

#ifdef OUTPUT_TO_FILE
	fprintf(output, "\n\n-------------------------------------\n\n");
#endif

	// Iterate over every pair of particles to calculate the forces
	//UART_Write("Computing forces..\n");
	for (int i = 0; i < totalOfParticles; i++)
	{
		LED_PROGRESS_BAR(i,totalOfParticles);

		particles[i].yForce = gravity;
		particles[i].xForce = 0;

		for (int j = 0; j < totalOfParticles; j++)
		{

#if USE_FLOAT==0
			//xParticleDistance = fix16_sub(particles[i].xPos, particles[j].xPos);
			//yParticleDistance = fix16_sub(particles[i].yPos, particles[j].yPos);
			////particlesDistance = fix16_smul(xParticleDistance,xParticleDistance) + fix16_smul(yParticleDistance,yParticleDistance);
			//particlesDistance = fix16_vec_mod(xParticleDistance, yParticleDistance);
			//particlesInteraction = fix16_sdiv(particlesDistance, fix16_one * 2) - fix16_one;

			//// force is updated only if particles are close enough
			//if (fix16_to_int(fix16_one - particlesInteraction) > 0)
			//{
			//particles[i].xForce = fix16_add(particles[i].xForce, fix16_sdiv(fix16_smul(particlesInteraction, fix16_add(fix16_smul(fix16_smul(xParticleDistance, fix16_sub(fix16_sub(3 * fix16_one, particles[i].density), particles[j].density)), pressure),
			//	fix16_smul(fix16_sub(particles[i].xVelocity, particles[j].xVelocity), viscosity))), particles[i].density));
			//
			//particles[i].yForce = fix16_add(particles[i].yForce, fix16_sdiv(fix16_smul(particlesInteraction, fix16_add(fix16_smul(fix16_smul(yParticleDistance, fix16_sub(fix16_sub(3 * fix16_one, particles[i].density), particles[j].density)), pressure),
			//	fix16_smul(fix16_sub(particles[i].yVelocity, particles[j].yVelocity), viscosity))), particles[i].density));
			//	
			//}

			double xDist = fix16_to_dbl(fix16_sub(particles[i].xPos, particles[j].xPos));
			double yDist = fix16_to_dbl(fix16_sub(particles[i].yPos, particles[j].yPos));
			//particlesDist = fix16_smul(xDist,xDist) + fix16_smul(yDist,yDist);
			double particlesDist = sqrt(xDist*xDist + yDist * yDist);
			double particlesInterac = particlesDist / 2.0 - 1.0;

			// force is updated only if particles are close enough
			if (floor(1.0 - particlesInterac) > 0)
			{
				particles[i].xForce = fix16_add(particles[i].xForce, fix16_from_dbl(particlesInterac * (xDist * (3 - fix16_to_dbl(particles[i].density) - fix16_to_dbl(particles[j].density)) * fix16_to_dbl(pressure) + fix16_to_dbl(particles[i].xVelocity) *
					fix16_to_dbl(viscosity) - fix16_to_dbl(particles[j].xVelocity) * fix16_to_dbl(viscosity)) / fix16_to_dbl(particles[i].density)));
				particles[i].yForce = fix16_add(particles[i].yForce, fix16_from_dbl(particlesInterac * (yDist * (3 - fix16_to_dbl(particles[i].density) - fix16_to_dbl(particles[j].density)) * fix16_to_dbl(pressure) + fix16_to_dbl(particles[i].yVelocity) *
					fix16_to_dbl(viscosity) - fix16_to_dbl(particles[j].yVelocity) * fix16_to_dbl(viscosity)) / fix16_to_dbl(particles[i].density)));
			}

#elif USE_FLOAT==1
			xParticleDistance = particles[i].xPos - particles[j].xPos;
			yParticleDistance = particles[i].yPos - particles[j].yPos;
			//particlesDistance = fix16_smul(xParticleDistance,xParticleDistance) + fix16_smul(yParticleDistance,yParticleDistance);
			particlesDistance = sqrt(xParticleDistance*xParticleDistance + yParticleDistance * yParticleDistance);
			particlesInteraction = particlesDistance / 2.0 - 1.0;

			// force is updated only if particles are close enough
			if (floor(1.0 - particlesInteraction) > 0)
			{
				particles[i].xForce += particlesInteraction * (xParticleDistance * (3 - particles[i].density - particles[j].density) * pressure + (particles[i].xVelocity - particles[j].xVelocity) * viscosity) / particles[i].density;
				particles[i].yForce += particlesInteraction * (yParticleDistance * (3 - particles[i].density - particles[j].density) * pressure + (particles[i].yVelocity - particles[j].yVelocity) * viscosity) / particles[i].density;
			}
#endif

		}
#ifdef OUTPUT_TO_FILE
#if USE_FLOAT==0
		fprintf(output, "Force %d: [%f, %f]\n", i, fix16_to_dbl(particles[i].xForce), fix16_to_dbl(particles[i].yForce));
#elif USE_FLOAT==1
		fprintf(output, "Force %d: [%f, %f]\n",i, particles[i].xForce, particles[i].yForce);
#endif
#endif
	}

#ifdef OUTPUT_TO_FILE
	fprintf(output, "\n\n-------------------------------------\n\n");
#endif

	// Empty the buffer
	memset(screenBuffer,0,BUFFER_SIZE);
	//UART_Write("Updating velocities and positions...\n");
	for (int i = 0; i < totalOfParticles; i++)
	{
		LED_PROGRESS_BAR(i,totalOfParticles);

		if (particles[i].wallflag==0)
		{
			// This is the newtonian mechanics part: knowing the force vector acting on each
			// particle, we accelerate the particle (see the change in velocity).
			// In turn, velocity changes the position at each tick.
			// Position is the integral of velocity, velocity is the integral of acceleration and
			// acceleration is proportional to the force.

			// Force affects velocity
#if USE_FLOAT==0
			if ( fix16_vec_mod(particles[i].xForce, particles[i].yForce) < (fix16_t)0b000001000011001100110011 )//fix16_sdiv(411,25))
			{
				particles[i].xVelocity = fix16_add(particles[i].xVelocity, fix16_sdiv(particles[i].xForce, 10*fix16_one));
				particles[i].yVelocity = fix16_add(particles[i].yVelocity, fix16_sdiv(particles[i].yForce, 10*fix16_one));
			}
			else
			{
				particles[i].xVelocity = fix16_add(particles[i].xVelocity, fix16_sdiv(particles[i].xForce, 11*fix16_one));
				particles[i].yVelocity = fix16_add(particles[i].yVelocity, fix16_sdiv(particles[i].yForce, 11*fix16_one));
			}

			// Velocity affects position
			particles[i].xPos = fix16_add(particles[i].xPos, particles[i].xVelocity);
			particles[i].yPos = fix16_add(particles[i].yPos, particles[i].yVelocity);

#elif USE_FLOAT==1
			if (sqrt(particles[i].xForce*particles[i].xForce + particles[i].yForce*particles[i].yForce) < 4.2)//fix16_sdiv(411,25))
			{
				particles[i].xVelocity += particles[i].xForce/10;
				particles[i].yVelocity += particles[i].yForce/10;
			}
			else
			{
				particles[i].xVelocity += particles[i].xForce/11;
				particles[i].yVelocity += particles[i].yForce/11;
			}

			// Velocity affects position
			particles[i].xPos +=  particles[i].xVelocity;
			particles[i].yPos +=  particles[i].yVelocity;

#endif

#ifdef OUTPUT_TO_FILE
#if USE_FLOAT==0
			fprintf(output, "Position %d: [%f, %f]\n", i, fix16_to_dbl(particles[i].xPos), fix16_to_dbl(particles[i].yPos));
#elif USE_FLOAT==1
			fprintf(output, "Position %d: [%f, %f]\n", i, particles[i].xPos, particles[i].yPos);
#endif
#endif
		}

#if USE_FLOAT==0
		// Given the position of the particle, determine the screen buffer
		// position that it's going to be in.
		uint32_t x = fix16_to_int(particles[i].xPos);
		// y scale correction, since each cell of the input map has
		// "2" rows in the particle space.
		uint32_t y = fix16_to_int(fix16_sdiv(particles[i].yPos,2*fix16_one));
#elif USE_FLOAT==1
		uint32_t x = (uint32_t)(particles[i].xPos);
		uint32_t y = (uint32_t)(particles[i].yPos/2);
#endif
		// if the particle is on screen, update
		// four buffer cells around it
		// in a manner of a "gradient",
		// the representation of 1 particle will be like this:
		//
		//      8 4
		//      2 1
		//
		// which after the lookup that puts chars on the
		// screen will look like:
		//
		//      ,.
		//      `'
		//
		// With this mechanism, each particle creates
		// a gradient over a small area (four screen locations).
		// As the gradients of several particles "mix",
		// (because the bits are flipped
		// independently),
		// a character will be chosen such that
		// it gives an idea of what's going on under it.
		// You can see how corners can only have values of 8,4,2,1
		// which will have suitably "pointy" characters.
		// A "long vertical edge" (imagine two particles above another)
		// would be like this:
		//
		//      8  4
		//      10 5
		//      2  1
		//
		// and hence 5 and 10 are both vertical bars.
		// Same for horizontal edges (two particles aside each other)
		//
		//      8  12 4
		//      2  3  1
		//
		// and hence 3 and 12 are both horizontal dashes.
		// ... and so on for the other combinations such as
		// particles placed diagonally, where the diagonal bars
		// are used, and places where four particles are present,
		// in which case the highest number is reached, 15, which
		// maps into the blackest character of the sequence, '#'

		if (y >= 0 && y < CONSOLE_HEIGHT-1  && x >= 0 && x < CONSOLE_WIDTH-1 )
		{
			uint32_t screenBufferIndex = x + CONSOLE_WIDTH * y;

//			UART_Write("X");
//			printLeds16(x);
//			UART_Write("Y");
//			printLeds16(y);


//			char num[10];
//			itoa(x,num,10);
//			UART_Write(num);
//			UART_Write(' - ');
//			itoa(y,num,10);
//			UART_Write(num);
//			UART_UART_WriteChar('|\t');

			screenBuffer[screenBufferIndex]   |= 8; // set 4th bit to 1
			screenBuffer[screenBufferIndex+1] |= 4; // set 3rd bit to 1
			// now the cell in row below
			screenBuffer[screenBufferIndex + CONSOLE_WIDTH]   |= 2; // set 2nd bit to 1
			screenBuffer[screenBufferIndex + CONSOLE_WIDTH + 1] |= 1; // set 1st bit to 1
		}

	}

	// Update the screen buffer
	//UART_Write("Rendering...\n");
	for (int i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++)
	{
		LED_PROGRESS_BAR(i,CONSOLE_WIDTH * CONSOLE_HEIGHT);

		if ( i % CONSOLE_WIDTH == CONSOLE_WIDTH - 1)
			screenBuffer[i] = '\n';
		else
			// the string below contains 16 characters, which is for all
			// the possible combinations of values in the screenbuffer since
			// it can be subject to flipping of the first 4 bits
			screenBuffer[i] = " '`-.|//,\\|\\_\\/#"[screenBuffer[i]];
			// ---------------------- the mappings --------------
			// 0  maps into space
			// 1  maps into '    2  maps into `    3  maps into -
			// 4  maps into .    5  maps into |    6  maps into /
			// 7  maps into /    8  maps into ,    9  maps into \
			// 10 maps into |    11 maps into \    12 maps into _
			// 13 maps into \    14 maps into /    15 maps into #
	}

}



