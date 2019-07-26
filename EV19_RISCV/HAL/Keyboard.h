#ifndef __PS2_KEYBOARD_H__
#define __PS2_KEYBOARD_H__
#include "../HAL/PS2.h"

#define KB_RESET 0xFF
#define KB_SET_DEFAULT 0xF6
#define KB_DISABLE 0xF5
#define KB_ENABLE 0xF4
#define KB_SET_TYPE_RATE_DELAY 0xF3
#define KB_Type PS2_Type
/**
 * @brief The enum type for the type of keyboard code received
 **/
typedef enum
{
	/** @brief Make code that corresponds to an ASCII character. For example, the ASCII make code for key <tt>[ A ] </tt> is 1C.
	 */
	KB_ASCII_MAKE_CODE = 1, 
	/** @brief Make code that corresponds to a non-ASCII character. For example, the binary (non-ASCII) make code for key <tt> [Left Alt]</tt> is 11.
	 */
	KB_BINARY_MAKE_CODE = 2,
	/** @brief Make code that has two bytes (the first byte is E0). For example, the long binary make code for key <tt>[Right Alt]</tt> is "E0 11".
	 */
	KB_LONG_BINARY_MAKE_CODE = 3,
	/** @brief Break code that has two bytes (the first byte is F0). For example, the break code for key <tt>[ A ]</tt> is "F0 1C".
	 */
	KB_BREAK_CODE = 4,
	/** @brief Long break code that has three bytes (with the first two bytes "E0 F0"). For example, the long break code for key <tt>[Right Alt]</tt> is "E0 F0 11".
	 */
	KB_LONG_BREAK_CODE = 5,
	/** @brief Scan codes that the decoding FSM is unable to decode.
	 */
	KB_INVALID_CODE = 6
} KB_CODE_TYPE;

/**
 * @brief Communicate with the PS/2 keyboard and get the make code of the key when a key is pressed.
 *
 * @param ps2 -- the PS/2 device structure. The actually connected PS/2 device has to be a keyboard otherwise the function's behavior is undefined.
 * @param decode_mode -- indicates which type of code (Make Code, Break Code, etc.) is received from the keyboard when the key is pressed.
 * @param buf  -- points to the location that stores the make/break code of the key pressed.
 * @param ascii -- pointer to the memory location to store the pressed ASCII character. If a non-ASCII key is pressed, \em ascii will be set to 0
 * 
 * @note For \c KB_LONG_BINARY_MAKE_CODE and \c KB_BREAK_CODE, only the second byte is returned. For \c KB_LONG_BREAK_CODE, only the third byte is returned.
 *
 * @return 0 for success, or negative errno for corresponding errors.
 **/
int Keyboard_Decode(KB_Type * base, KB_CODE_TYPE *decode_mode, uint8_t *buf, char *ascii);

/**
 * @brief Set the repeat/delay rate of the keyboard.
 *
 * @param rate -- an 8-bit number that represents the repeat/delay rate of the keyboard.
 *
 * @return 0 on success, negative value on error. 
 **/
uint32_t Keyboard_SetRate(KB_Type * base, uint8_t rate);

/**
 * @brief Translate the make code into string description
 *
 * @param decode_mode -- the type of the make code (ASCII, binary, or long binary)
 * @param makecode -- the last byte of the make code (if the make code has multiple bytes)
 * @param str -- the pointer to the memory location to store the description string 
 *
 **/
void Keyboard_TranslateCode(KB_CODE_TYPE decode_mode, uint8_t makecode, char *str);

/**
 * @brief Send the reset command to the keyboard.
 *
 * @return 0 on passing the BAT (Basic Assurance Test), negative value on error.
 **/
uint32_t Keyboard_Reset(KB_Type * base);

#endif
