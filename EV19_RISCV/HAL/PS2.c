#include "../HAL/PS2.h"

#include <errno.h>

#define PS2_ACK 	(0xFA)
#define TIMEOUT     (1000U)

int PS2_Init(PS2_Type * base,PS2_Device_t * dev)
{
	uint8_t * leds= (uint8_t *)LEDS_BASE;
	// initialize the device
	uint8_t byte;
	//send the reset request, wait for ACK
	int status = PS2_WriteByteWithAck(base, 0xff);
	if (status == 0)
	{
		// reset succeed, now try to get the BAT result, AA means passed
		status = PS2_ReadByteTimeout(base, &byte);
		if (status == 0 && byte == 0xAA)
		{
			//get the 2nd byte
			status = PS2_ReadByteTimeout(base, &byte);
			if (status == -ETIMEDOUT)
			{
				//for keyboard, only 2 bytes are sent(ACK, PASS/FAIL), so timeout
				(*dev) = PS2_KEYBOARD;
			}
			else if (status == 0 && byte == 0x00)
			{
				//for mouse, it will sent out 0x00 after sending out ACK and PASS/FAIL.
				(*dev) = PS2_MOUSE;
				(void) PS2_WriteByte(base, 0xf4); // enable data from mouse
			}
			else
				(*dev) = PS2_UNKNOWN;
		}
		//else (*leds)=0xFF;
		return 0;
	}
	//else (*leds)=0xF0;
	return status;
}

void PS2_EnableInterrupts(PS2_Type * base)
{
	// set RE to 1 while maintaining other bits the same
	base->control |= PS2_RE_MASK;
}

void PS2_DisableInterrupts(PS2_Type * base)
{
	// set RE to 0 while maintaining other bits the same
	base->control &= ~PS2_RE_MASK;
}

int PS2_WriteByte(PS2_Type * base, uint8_t byte)
{
	base->data = PS2_DATA(byte);

	uint32_t ctrl = base->control;
	if ( ((ctrl&PS2_CE_MASK)>>PS2_CE_SHIFT)  == 0x1u )
	{
		//CE bit is set --> error occurs on sending commands
		return -EIO;
	}
	return 0;
}

int PS2_WaitAck(PS2_Type * base)
{
	uint8_t data = 0;
	uint8_t status = 0;
	uint8_t * leds= (uint8_t *)LEDS_BASE;

	do
	{
		status = PS2_ReadByteTimeout(base, &data); 
		if ( status == 0)
		{
			//(*leds)=0xF0;
			if (data == PS2_ACK)
				return 0;
		}
		else 
		{
			//(*leds)=0xFF;
			return status;
		}
	} while(1);
	return -ETIMEDOUT;
}

int PS2_WriteByteWithAck(PS2_Type * base, uint8_t byte)
{
	uint8_t * leds= (uint8_t *)LEDS_BASE;

	int send_status = PS2_WriteByte(base, byte);
	if ( send_status != 0)
	{
		//(*leds)=0xF0;
		return send_status;
	}

	int ack_status = PS2_WaitAck(base);
	if(ack_status!=0)
		//(*leds)=0xFF;
	return ack_status;
}

int PS2_ReadByteTimeout(PS2_Type * base, uint8_t *byte)
{
	uint8_t * leds= (uint8_t *)LEDS_BASE;
	uint32_t data_reg = 0;
	uint32_t count = 0;
	do {
		count++;
		data_reg = base->data;

		if ( (data_reg&PS2_RVALID_MASK)>>PS2_RVALID_SHIFT )
		{
			//(*leds)=8;
			*byte = (uint8_t)((data_reg&PS2_DATA_MASK)>>PS2_DATA_SHIFT);
			(*leds)=8;
			return 0;
		}
		//timeout = 0 means to disable the timeout
		if ( TIMEOUT != 0 && count > TIMEOUT)
		{
			(*leds)=0xFF;
			return -ETIMEDOUT;
		}
	} while (1);
}

int PS2_ReadByte(PS2_Type * base, uint8_t *byte)
{
	uint8_t * leds= (uint8_t *)LEDS_BASE;
	uint32_t data_reg = base->data;

	//(*leds)=((data_reg>>PS2_RVALID_SHIFT)&PS2_RVALID_MASK);

	if (((data_reg&PS2_RVALID_MASK)>>PS2_RVALID_SHIFT))
	{
		*byte = (uint8_t)((data_reg&PS2_DATA_MASK)>>PS2_DATA_SHIFT);
		return 0;
	}
	return -1;
}

void PS2_ClearFifo(PS2_Type * base)
{
	// The DATA byte of the data register will be automatically cleared after a read
	// So we simply keep reading it until there are no available bytes
	uint16_t num = 0;
	unsigned int data_reg = 0;
	do
	{
		// read the data register (the DATA byte is cleared)
		data_reg = base->data;
		// get the number of available bytes from the RAVAIL part of data register
		num = (uint16_t)((data_reg&PS2_RAVAIL_MASK)>>PS2_RAVAIL_SHIFT);
	} while (num > 0);
}

//////////////////////////////////////////////////////////////
// FD Functions
int PS2_ReadByter (PS2_Type * base, char* ptr, int nBytes)
{
	int status = 0;
	int count = 0;
	while (count < nBytes)
	{
		status = PS2_ReadByteTimeout(base, (uint8_t *)ptr++);
		if (status!=0)
			return count;
		count++;
	} 
	return count;
}

int PS2_WriteBytes (PS2_Type * base, const char* ptr, int nBytes)
{
	int status = 0;
	int count = 0;
	while (count < nBytes)
	{
		status = PS2_WriteByte(base, *(ptr++) );
		if (status!=0)
			return count;
		count++;
	}
	return count;
}
