#ifndef HAL_JTAG_UART_H_
#define HAL_JTAG_UART_H_

#include <stdbool.h>
#include <stdint.h>

void UART_WriteChar(char c);

int UART_Write(const char * ptr);

int UART_WriteLen(const char * ptr,uint32_t len);

char UART_ReadChar();

bool UART_ReadCharNB(char* c);

int UART_Read(char* buffer, int len, int flags);

void UART_ClearReadFifo();

#endif /* HAL_JTAG_UART_H_ */
