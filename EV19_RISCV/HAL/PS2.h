#ifndef PS2_H
#define PS2_H

#include <stdint.h>

#include "../HAL/EV19_Peripherals.h"

typedef enum {
	/// @brief Indicate that the device is a PS/2 Mouse.
	PS2_MOUSE = 0,
	/// @brief Indicate that the device is a PS/2 Keyboard.
	PS2_KEYBOARD = 1,
	/// @brief The program cannot determine what type the device is.
	PS2_UNKNOWN = 2
} PS2_Device_t;

/*
 * Device structure definition. Each instance of the driver uses one
 * of these structures to hold its associated state.
 */
typedef struct {
	/// @brief the default timeout value.
	unsigned int timeout;
	/// @brief the device type (PS/2 Mouse or PS/2 Keyboard).
	PS2_Device_t device_type;
} PS2_t;


//////////////////////////////////////////////////////////////////////////
// HAL system functions

/**
 * @brief Initialize the PS/2 device and detect device type (mouse or keyboard). 
 *
 * @param ps2 -- the PS/2 device structure.
 * 
 * @note The function will set the \c device_type field of \em ps2 to \c
 * PS2_MOUSE or \c PS2_KEYBOARD upon successful initialization, otherwise the
 * intialization is unsuccessful.
 *
 **/
int PS2_Init(PS2_Type * base,PS2_Device_t * dev);

/**
 * @brief Enable read interrupts for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 * @return nothing
 **/
void PS2_EnableInterrupts(PS2_Type * base);

/**
 * @brief Diaable read interrupts for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 * @return nothing
 **/
void PS2_DisableInterrupts(PS2_Type * base);

/**
 * @brief Write a byte to the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- the byte to be written to the PS/2 port.
 *
 * @return 0 on success, or \c -EIO on failure.
 **/
int PS2_WriteByte(PS2_Type * base, uint8_t byte);

/**
 * @brief Write a byte to the PS/2 port and wait for the acknowledgment.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- the byte to be written to the PS/2 port.
 *
 * @return 0 on success, \c -EIO on write failure, or \c -ETIMEDOUT on timeout when waiting for the acknowledgment.
 * @note The timeout value is defined in the PS/2 device structure \c PS2_dev.
 **/
int PS2_WriteByteWithAck(PS2_Type * base, uint8_t byte);


int PS2_WaitAck(PS2_Type * base);


/**
 * @brief Read a byte from the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- pointer to the memory location to store the byte.
 *
 * @return 0 on success, else -1
 *
 **/
int PS2_ReadByte(PS2_Type * base, uint8_t *byte);

/**
 * @brief Read a byte from the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- pointer to the memory location to store the byte.
 *
 * @return 0 on success, or \c -ETIMEDOUT when timeout.
 *
 * @note This function waits for a timeout period if there is no data available.
 **/
int PS2_ReadByteTimeout(PS2_Type * base, uint8_t *byte);

/**
 * @brief Clear the FIFO for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 **/
void PS2_ClearFifo(PS2_Type * base);

/**
 * @brief Read \em nBytes bytes from the PS/2 device.
 *
 * @param ptr -- memory location to store the bytes read.
 * @param len -- number of bytes to be read.
 *
 * @return the number of bytes actually read.
 *
 **/
int PS2_ReadByter (PS2_Type * base, char* ptr, int nBytes);

/**
 * @brief Write \em nBytes bytes to the PS/2 device from memory location pointed by \em ptr.
 *
 * @param fd -- the file descriptor for the PS/2 device.
 * @param ptr -- memory location storing the bytes to write.
 * @param len -- number of bytes to write.
 *
 * @return the number of bytes actually written.
 **/
int PS2_WriteBytes (PS2_Type * base, const char* ptr, int nBytes);


#endif /* PS2_H */


