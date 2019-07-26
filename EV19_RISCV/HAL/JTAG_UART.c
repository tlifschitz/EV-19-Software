#include "JTAG_UART.h"
#include "errno.h"
#include "EV19_Peripherals.h"

#ifdef Win32
#include <stdio.h>
#endif


void UART_WriteChar(char c)
{
#ifdef Win32
	putchar(c);
#else
	// Wait for space in fifo
	while(JTAG_UART_WSPACE_READ(JTAG_UART->control) == 0);
    JTAG_UART->data = JTAG_UART_DATA(c);
#endif

}


int UART_Write(const char * ptr)
{
 // const char * end = ptr + len;
#ifdef Win32
	printf("%s",ptr);
#else
  while (*ptr)
    if (JTAG_UART_WSPACE_READ(JTAG_UART->control) != 0)
      JTAG_UART->data = JTAG_UART_DATA(*ptr++);
#endif
  return 0;//ptr+count-end;
}

int UART_WriteLen(const char * ptr,uint32_t len)
{
  const char * end = ptr + len;

  while (ptr<end)
#ifdef Win32
	  putchar(ptr++);
#else
    if (JTAG_UART_WSPACE_READ(JTAG_UART->control) != 0)
      JTAG_UART->data = JTAG_UART_DATA(*ptr++);
#endif

  return 0;//ptr+count-end;
}


char UART_ReadChar()
{
#ifdef Win32
	return getchar();
#else
	uint32_t data;
	do
	{
		data= JTAG_UART->data;
	}while((data & JTAG_UART_RVALID_MASK)==0);
	return JTAG_UART_DATA_READ(data);
#endif
}
void UART_ClearReadFifo()
{
#ifdef Win32

#else
	uint32_t data;
	do
	{
		data= JTAG_UART->data;
	}while(JTAG_UART_RAVAIL_READ(data)==0);
#endif
}

bool UART_ReadCharNB(char* c)
{
#ifdef Win32

#else
	unsigned int data = JTAG_UART->data;

    if (data & JTAG_UART_RVALID_MASK)
    {
    	*c = JTAG_UART_DATA_READ(data);
    	return true;
    }
    else
    	return false;
#endif
}

int UART_Read(char* buffer, int len, int flags)
{
	char * ptr = buffer;
	char * end = buffer + len;

	while (ptr < end)
	{
#ifdef Win32
		*ptr++ = getchar();
#else
		unsigned int data = JTAG_UART->data;

		if (data & JTAG_UART_RVALID_MASK)
			*ptr++ = JTAG_UART_DATA_READ(data);
		else if (ptr != buffer)
			break;
#endif
	}

  if (ptr != buffer)
    return ptr - buffer;
  else
    return -EIO;
}
