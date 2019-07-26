#include "HAL/EV19_Peripherals.h"
#include "HAL/JTAG_UART.h"
#include "Mandelbrot.h"
#include "Scope.h"
#include <stdlib.h>


uint32_t getInt()
{
	char input[11];
	int i=0;
	while((input[i++]=UART_ReadChar()) !='\r') UART_WriteChar(input[i-1]);
	input[i-1]='\0';
	return atoi(input);
}


int main()
{

	//ScopeMain();

	interactiveMandelbrot();

//
//
//
//
//	UART_Write("Number of frames: ");
//	uint8_t nFrames = getInt();
//
//	malloc_addblock((void*)(0x4000000),0x2000000);
//
//	a = malloc(97687 * sizeof(double complex));
//	if(a==NULL)
//	{
//		UART_Write("ERROR (a)\n");
//		while(1);
//	}
//
//	b = malloc(6856*sizeof(char));
//	if(b==NULL)
//	{
//		UART_Write("ERROR (b)\n");
//		while(1);
//	}
//
//	char* frames = malloc(nFrames*6856*sizeof(char));
//	if(frames==NULL)
//	{
//		UART_Write("ERROR (frames)\n");
//		while(1);
//	}
//
//compute:
//	//a = memset((void*)a,0,97687 * sizeof(double complex));
//	r = a;
//
//	//b = memset((void*)b,0,6856*sizeof(char));
//	o = b;
//
//
//	UART_Write("Paste your input\n");
//	while( (x = UART_ReadChar()) != '\t' )
//	{
//		if(x=='\r') x='\n';
//		UART_WriteChar(x);
//		w = x > 10 ? 32 < x ? 4[*r++ = w, r] = w + 1, *r = r[5] = x == 35, r += 9 : 0, w - I : (x = w + 2);;
//	}
//
//	UART_Write("\n\n");
//
//	char * frame = frames;
//
//	for(int i=0; i<nFrames; i++)
//	{
//		LED_PROGRESS_BAR(i,nFrames);
//		char progress[4] = "000";
//		itoa(i * 100 / nFrames, progress, 10);
//		UART_Write("\rRendering: ");
//		UART_Write(progress);
//		UART_Write("%");
//
//		for (p = a; p[2] = p[1] * 9, p < r; p += 5)
//			for (q = a; w = cabs(d = *p - *q) / 2 - 1, q < r; q += 5)
//				if (0 < (x = 1 - w))
//					p[2] += w * w;
//
//		for (p = a; p[3] = G, p < r; p += 5)
//			for (q = a; w = cabs(d = *p - *q) / 2 - 1, q < r; q += 5)
//				if (0 < (x = 1 - w))
//					p[3] += w * (d * (3 - p[2] - q[2]) * P + p[4] * V - q[4] * V) / p[2];
//
//		for (x = 0; 2012 - 1 > x++;)
//			b[x] = 0;
//
//		for (p = a; (t = b + 10 + (x = *p * I) + 80 * (y = *p / 2), *p += p[4] += p[3] / 10 * !p[1]), p < r; p += 5)
//			x = 0 <= x && x < 79 && 0 <= y && y < 23 ? 1[1[*t |= 8, t] |= 4, t += 80] = 1, *t |= 2 : 0;
//
//		for (x = 0; 2012 - 1 > x++;)
//			b[x] = " '`-.|//,\\" "|\\_" "\\/\x23\n"[x % 80 - 9 ? x[b] : 16];;
//
//		 strcpy(frame,o);
//		 o = b + 4;
//		 frame += 6856*sizeof(char);
//	}
//
//uint32_t delayCount = 30000;
//
//playAnimation:
//	frame = frames;
//
//	for(int i=0; i<nFrames; i++)
//	{
//		UART_Write(frame);
//		UART_Write("\n\n\n\n");
//		frame += 6856*sizeof(char);
//
//		uint32_t n = delayCount;
//		while(n--) asm("nop");
//	}
//
//	while(1)
//	{
//		switch(UART_ReadChar())
//		{
//		case 'p':
//			goto playAnimation;
//			break;
//		case 'n':
//			nFrames = getInt();
//			goto compute;
//			break;
//		case 'd':
//			delayCount = getInt();
//			*LEDS = delayCount;
//			goto playAnimation;
//			break;
//		}
//	}
//	while(*BUTTON==1);
//	while(*BUTTON==0);
//	goto playAnimation;

	return 0;
}
