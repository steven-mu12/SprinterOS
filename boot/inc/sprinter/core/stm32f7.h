/**
 ******************************************************************************
 * @file           : stm32f7.h
 * @author         : Steven Mu
 * @summary		   : File for common macros and memory address definitions
 ******************************************************************************
 * MIT License

 * Copyright (c) 2024 Steven Mu

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 ******************************************************************************
 */

#ifndef __STM32F7_H__
#define __STM32F7_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "sprinter/core/cmsis_device.h"
#include "external/cmsis/core_cm7.h"

/** 
 * @brief SINGLE BITWISE OPERATION MACROS 
 */
#define SET_BITMASK(BIT)					(1U << BIT)					/* use | with reg */
#define RESET_BITMASK(BIT)					(~(1U << BIT))				/* use & with reg */
#define READ_BIT(INPUT, BIT)				(((INPUT) & (1U << BIT)) >> BIT)
#define SET_BIT(INPUT, BIT)                 (INPUT = (INPUT) | (SET_BITMASK(BIT)))
#define RESET_BIT(INPUT, BIT)               (INPUT = (INPUT) & (RESET_BITMASK(BIT)))

#define CLEAR_BITS_MASK(MASK, BIT)			(~(MASK << BIT)) 			/* use & with reg */
#define INSERT_BITS_MASK(MASK, BIT)			(MASK << BIT)	 			/* use | with reg */
#define SET_BITS(INPUT, BIT, PATTERN, MASK)	(INPUT = ( (INPUT & (CLEAR_BITS_MASK(MASK, BIT))) |  \
                                                                (INSERT_BITS_MASK(PATTERN, BIT)) \
													 )                                           \
                                            )
#define READ_BITS(INPUT, BIT, MASK)			((INPUT & (MASK << BIT)) >> BIT)

/** 
 * @brief GENERAL DEFINITIONS
 */
#define HIGH							1
#define LOW								0

/** 
 * @brief MMIO ADDRESSES
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
 * @brief PREDEFINED, GLOBALLY KNOWN ADDRESSES
 */
typedef struct BASIC_TIM BASIC_TIM;
extern BASIC_TIM* __global_simple_timer_ptr__;

#endif
