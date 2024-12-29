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

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************
 */

#ifndef __STM32F7_H__
#define __STM32F7_H__

#include <stdint.h>

/* SINGLE BITWISE OPERATION MACROS */
#define SET_BITMASK(BIT)					(1U << BIT)					/* use | with reg */
#define RESET_BITMASK(BIT)					(~(1U << BIT))				/* use & with reg */
#define READ_BIT(INPUT, BIT)				(((INPUT) & (1U << BIT)) >> BIT)

#define CLEAR_BITS_MASK(MASK, BIT)			(~(MASK << BIT)) 			/* use & with reg */
#define INSERT_BITS_MASK(MASK, BIT)			(MASK << BIT)	 			/* use | with reg */
#define SET_BITS(INPUT, BIT, NEW, MASK)		(INPUT = ( (INPUT & (CLEAR_BITS_MASK(MASK, BIT))) | \
										 	 	 	   (INSERT_BITS_MASK(NEW, BIT)) ) \
											)

/* GENERAL DEFINITIONS */
#define HIGH							1
#define LOW								0

/* MMIO ADDRESSES */
#define FLASH_ADDRESS					0x40023C00
#define RCC_ADDRESS						0x40023800

#endif