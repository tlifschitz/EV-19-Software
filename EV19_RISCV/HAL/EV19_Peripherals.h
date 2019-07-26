#ifndef __EV19_PERIPHERALS__
#define __EV19_PERIPHERALS__

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            ID                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define ID_BASE                     (0x01000000u)
#define ID ((ID_Type*)ID_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            LEDS                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef volatile uint8_t Leds_Type;
#define LEDS_BASE                   (0x01001000u)
#define LEDS ((Leds_Type*)LEDS_BASE)

#define LED_PROGRESS_BAR(n,N) *LEDS = (uint8_t)( ( 1 << (n*8/N) ) - 1 )
//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           DIP                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define DIP_BASE                    (0x01001100u)
#define DIP ((DIP_Type*)DIP_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          BUTTON                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef volatile uint8_t Button_Type;
#define BUTTON_BASE                 (0x01001200u)
#define BUTTON ((Button_Type*)BUTTON_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          PLL                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define PLL_BASE                    (0x01001300u)
#define PLL ((PLL_Type*)PLL_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           ADC                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////

typedef uint32_t ADC_Type;


#define ADC_BASE                    (0x01002000u)
#define ADC ((ADC_Type*)ADC_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         ACCEL                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define ACCEL_BASE                  (0x01003000u)
#define ACCEL ((ACCEL_Type*)ACCEL_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         TIMER                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	volatile int16_t status;
	volatile int16_t control;
	volatile int32_t period;
	volatile int32_t snap;
}Timer_Type;

#define TIMER_BASE                  (0x01004000u)
#define TIMER ((Timer_Type*)TIMER_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         COUNTERS                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define PERF_COUNTER_SECTIONS 4

typedef struct
{
	volatile uint32_t counterLo;
	volatile uint32_t counterHi;
	volatile uint32_t occurrencies;
	volatile uint32_t reserved;
}SectionCounter_Type;

#define PERF_COUNTERS_BASE               (0x01005000u)
#define PERF_COUNTERS ((SectionCounter_Type(*)[PERF_COUNTER_SECTIONS])PERF_COUNTERS_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            PS2                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    volatile uint32_t data;
    volatile uint32_t control;
}PS2_Type;


#define PS2_DATA_MASK        (0x000000FF)                       
#define PS2_DATA_SHIFT       (0U)                       
#define PS2_DATA(x)          (((uint32_t)(((uint32_t)(x)) << PS2_DATA_SHIFT)) & PS2_DATA_MASK)                  
//#define PS2_DATA_READ(x)     ((uint8_t) ((((uint32_t)(x)) >> PS2_DATA_SHIFT) & PS2_DATA_MASK))                  
  

#define PS2_RVALID_MASK      (0x00008000)                       
#define PS2_RVALID_SHIFT     (15U)                           
#define PS2_RVALID(x)        (((uint32_t)(((uint32_t)(x)) << PS2_RVALID_SHIFT)) & PS2_RVALID_MASK)  

#define PS2_RAVAIL_MASK      (0xFFFF0000)                       
#define PS2_RAVAIL_SHIFT     (16U)                           
#define PS2_RAVAIL(x)        (((uint32_t)(((uint32_t)(x)) << PS2_RAVAIL_SHIFT)) & PS2_RAVAIL_MASK)  
//#define PS2_RAVAIL_READ(x)   ((uint16_t) ((((uint32_t)(x)) >> PS2_RAVAIL_SHIFT) & PS2_RAVAIL_MASK))

#define PS2_RE_MASK          (0x00000001)                   
#define PS2_RE_SHIFT         (0U)                       
#define PS2_RE(x)            (((uint32_t)(((uint32_t)(x)) << PS2_RE_SHIFT)) & PS2_RE_MASK)  

#define PS2_RI_MASK          (0x00000100)                   
#define PS2_RI_SHIFT         (8U)                       
#define PS2_RI(x)            (((uint32_t)(((uint32_t)(x)) << PS2_RI_SHIFT)) & PS2_RI_MASK)  

#define PS2_CE_MASK          (0x00000400)                   
#define PS2_CE_SHIFT         (10U)                       
#define PS2_CE(x)            (((uint32_t)(((uint32_t)(x)) << PS2_CE_SHIFT)) & PS2_CE_MASK)  


#define PS20_BASE                   (0x01006000u)
#define PS20 ((PS2_Type*)PS20_BASE)
#define PS21_BASE                   (0x01006100u)
#define PS21 ((PS2_Type*)PS21_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           CHAR BUFFER                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))
#define LOG_4(n) (((n) >= 1<<4) ? (4 + LOG_2((n)>>4)) : LOG_2(n))
#define LOG_8(n) (((n) >= 1<<8) ? (8 + LOG_4((n)>>8)) : LOG_4(n))
#define LOG2(n)   (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))

#define CHAR_BUFFER_WIDTH  			(80U)
#define CHAR_BUFFER_HEIGHT 			(60U)

#define ADDRESSING_CONSECUTIVE		(0)
#define ADDRESSING_X_Y				(1)
#define CHAR_BUFFER_ADDRESSING		ADDRESSING_X_Y

#if CHAR_BUFFER_ADDRESSING==ADDRESSING_X_Y
	#define X_BITS						LOG2(CHAR_BUFFER_WIDTH)
	#define X_MASK						((1<<X_BITS)-1)

	#define Y_BITS						LOG2(CHAR_BUFFER_HEIGHT)
	#define Y_MASK						((1<<Y_BITS)-1)

	#define PUTCHAR(x,y,c) 				*( CHAR_BUFFER + ( ( ( (y) & Y_MASK ) << X_BITS ) | ( (x) & X_MASK ) ) ) = c
#else
	#define PUTCHAR(x,y,c) 				*( CHAR_BUFFER + (y) * CHAR_BUFFER_WIDTH + (x) ) = c
#endif // CHAR_BUFFER_ADDRESSING==ADDRESSING_X_Y


#define CHAR_BUFFER_CONTROL_BASE    (0x03007000u)
#define CHAR_BUFFER_CONTROL 		((CHAR_BUFFER_CONTROL_Type*)CHAR_BUFFER_CONTROL_BASE)

#define CHAR_BUFFER_BASE            (0x03008000u)
#define CHAR_BUFFER 				((char*)CHAR_BUFFER_BASE)




//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          PIXEL_BUFFER                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (480)

typedef int16_t RGB16_t;

#define RGB16(r,g,b) (RGB16_t)( ((((r*31)/255)<<11)&0b1111100000000000) | ((((g*63)/255)<<5)&0b0000011111100000) | (((b*31)/255)&0b0000000000011111))

#define PIXEL_BUFFER_CONTROL_BASE   (0x0300A000u)
#define PIXEL_BUFFER_CONTROL ((PIXEL_BUFFER_CONTROL_Type*)PIXEL_BUFFER_CONTROL_BASE)

#define PIXEL_BUFFER_BASE                (0x05000000u)
#define PIXEL_BUFFER 					 ((int16_t(*)[SCREEN_WIDTH])PIXEL_BUFFER_BASE)
#define PIXEL_BUFFER32 					 ((int32_t(*)[SCREEN_WIDTH/2])PIXEL_BUFFER_BASE)


#define PIXEL_BACK_BUFFER_BASE                (0x05200000u)
#define PIXEL_BACK_BUFFER 					  ((RGB16_t(*)[SCREEN_WIDTH])PIXEL_BACK_BUFFER_BASE)
#define PIXEL_BACK_BUFFER32 			      ((RGB16_t(*)[SCREEN_WIDTH])PIXEL_BACK_BUFFER_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        RGB RESAMPLER                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define RGB_RESAMPLER_BASE          (0x0300B000u)
#define RGB_RESAMPLER ((RGB_RESAMPLER_Type*)RGB_RESAMPLER_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            UART                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART_BASE                   (0x0100C000u)
#define UART ((UART_Type*)UART_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           SPI                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define SPI_BASE                    (0x0100D000u)
#define SPI ((SPI_Type*)SPI_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            I2C                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define I2C_BASE                    (0x0100E000u)
#define I2C ((I2C_Type*)I2C_BASE)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        JTAG_UART                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	volatile uint32_t data;
	volatile uint32_t control;
}JTAG_UART_Type;

#define JTAG_UART_BASE                    (0x0100C000u)
#define JTAG_UART ((JTAG_UART_Type*)JTAG_UART_BASE)

#define JTAG_UART_RAVAIL_MASK		(0xFFFF0000)
#define JTAG_UART_RAVAIL_SHIFT		(16U)
#define JTAG_UART_RAVAIL(x)			(((uint32_t)(((uint32_t)(x)) << JTAG_UART_RAVAIL_SHIFT )) & JTAG_UART_RAVAIL_MASK)
#define JTAG_UART_RAVAIL_READ(x)	((uint32_t)(((x) & JTAG_UART_RAVAIL_MASK ) >> JTAG_UART_RAVAIL_SHIFT))

#define JTAG_UART_RVALID_MASK		(0x00008000)
#define JTAG_UART_RVALID_SHIFT		(15U)
#define JTAG_UART_RVALID(x)			(((uint32_t)(((uint32_t)(x)) << JTAG_UART_RVALID_SHIFT )) & JTAG_UART_RVALID_MASK)
#define JTAG_UART_RVALID_READ(x)	((uint32_t)(((x) & JTAG_UART_RVALID_MASK ) >> JTAG_UART_RVALID_SHIFT))

#define JTAG_UART_DATA_MASK			(0x000000FF)
#define JTAG_UART_DATA_SHIFT		(0U)
#define JTAG_UART_DATA(x)			(((uint32_t)(((uint32_t)(x)) << JTAG_UART_DATA_SHIFT )) & JTAG_UART_DATA_MASK)
#define JTAG_UART_DATA_READ(x)		((uint32_t)(((x) & JTAG_UART_DATA_MASK ) >> JTAG_UART_DATA_SHIFT))

#define JTAG_UART_WSPACE_MASK		(0xFFFF0000)
#define JTAG_UART_WSPACE_SHIFT		(16U)
#define JTAG_UART_WSPACE(x)			(((uint32_t)(((uint32_t)(x)) <<  )) & )
#define JTAG_UART_WSPACE_READ(x)	((uint32_t)(((x) & JTAG_UART_WSPACE_MASK ) >> JTAG_UART_WSPACE_SHIFT))

#define JTAG_UART_AC_MASK			(0x00000400)
#define JTAG_UART_AC_SHIFT			(10U)
#define JTAG_UART_AC(x)				(((uint32_t)(((uint32_t)(x)) << JTAG_UART_AC_MASK )) & JTAG_UART_AC_SHIFT)
#define JTAG_UART_AC_READ(x)		((uint32_t)(((x) & JTAG_UART_AC_MASK ) >> JTAG_UART_AC_SHIFT))

#define JTAG_UART_WI_MASK			(0x00000200)
#define JTAG_UART_WI_SHIFT			(9U)
#define JTAG_UART_WI(x)				(((uint32_t)(((uint32_t)(x)) << JTAG_UART_WI_MASK )) & JTAG_UART_WI_SHIFT)
#define JTAG_UART_WI_READ(x)		((uint32_t)(((x) & JTAG_UART_WI_MASK ) >> JTAG_UART_WI_SHIFT))

#define JTAG_UART_RI_MASK			(0x00000100)
#define JTAG_UART_RI_SHIFT			(8U)
#define JTAG_UART_RI(x)				(((uint32_t)(((uint32_t)(x)) << JTAG_UART_RI_MASK )) & JTAG_UART_RI_SHIFT)
#define JTAG_UART_RI_READ(x)		((uint32_t)(((x) & JTAG_UART_RI_MASK ) >> JTAG_UART_RI_SHIFT))

#define JTAG_UART_WE_MASK			(0x00000200)
#define JTAG_UART_WE_SHIFT			(2U)
#define JTAG_UART_WE(x)				(((uint32_t)(((uint32_t)(x)) << JTAG_UART_WE_MASK )) & JTAG_UART_WE_SHIFT)
#define JTAG_UART_WE_READ(x)		((uint32_t)(((x) & JTAG_UART_WE_MASK ) >> JTAG_UART_WE_SHIFT))

#define JTAG_UART_RE_MASK			(0x00000100)
#define JTAG_UART_RE_SHIFT			(1U)
#define JTAG_UART_RE(x)				(((uint32_t)(((uint32_t)(x)) << JTAG_UART_RE_MASK )) & JTAG_UART_RE_SHIFT)
#define JTAG_UART_RE_READ(x)		((uint32_t)(((x) & JTAG_UART_RE_MASK ) >> JTAG_UART_RE_SHIFT))





#define RAM_BASE		   (0x02000000)
#define SDRAM_BASE		   (0x04000000)

#endif //__EV19_PERIPHERALS__
