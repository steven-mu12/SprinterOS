#ifndef __STM32F7_H__
#define __STM32F7_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "sprinter/core/cmsis_device.h"

#include "external/cmsis/core_cm7.h"

/** 
 * Bitwise operation macros
 */
#define SET_BITMASK(BIT)					(1U << BIT)
#define RESET_BITMASK(BIT)					(~(1U << BIT))
#define READ_BIT(INPUT, BIT)                (((INPUT) >> (BIT)) & 1U)
#define SET_BIT(INPUT, BIT)                 (INPUT = (INPUT) | (SET_BITMASK(BIT)))
#define RESET_BIT(INPUT, BIT)               (INPUT = (INPUT) & (RESET_BITMASK(BIT)))
#define CLEAR_BITS_MASK(MASK, BIT)			(~(MASK << BIT))
#define INSERT_BITS_MASK(MASK, BIT)			(MASK << BIT)
#define SET_BITS(INPUT, BIT, PATTERN, MASK)	(INPUT = ( (INPUT & (CLEAR_BITS_MASK(MASK, BIT))) |  \
                                                                (INSERT_BITS_MASK(PATTERN, BIT)) \
													 )                                           \
                                            )
#define READ_BITS(INPUT, BIT, MASK)			((INPUT & (MASK << BIT)) >> BIT)

#define HIGH							1
#define LOW								0

/** 
 * MMIO addresses
 */
#define FLASH_ADDRESS					0x40023C00
#define RCC_ADDRESS						0x40023800
#define GPIO_BASE_ADDRESS				0x40020000
#define UART_1_BASE						0x40011000
#define IWDG_BASE                       0x40003000
#define BASIC_TIM_BASE                  0x40001000
#define SPI1_BASE                       0x40013000
#define SPI2_BASE                       0x40003800
#define SPI3_BASE                       0x40003C00
#define SPI4_BASE                       0x40013400

/** 
 * globally known variables - anything that includes this header will have access
 */
typedef struct BASIC_TIM BASIC_TIM;
extern BASIC_TIM* __global_simple_timer_ptr__;

#endif
