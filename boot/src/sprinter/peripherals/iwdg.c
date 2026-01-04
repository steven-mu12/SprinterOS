/**
 ******************************************************************************
 * @file           : iwdg.c
 * @author         : Steven Mu
 * @summary		   : Independent Watchdog Timer Functionalities
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

#include <stdint.h>

#include "sprinter/peripherals/iwdg.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals/rcc.h"

/**
 * @brief User Functions
 */

//! Init function for IWDG
int iwdg_init(void) {
	// init the LSI oscillator and wait for stable
	SET_BITS(RCC->CSR, 0, 0x01, 0x01);
	while (!READ_BIT(RCC->CSR, 1));

	// init the IWDG by writing special code to KR
	IWDG->KR = 0xCCCC;
	IWDG->KR = 0x5555;

	// set the prescaler to get ~ a 1000ms expiration
	while (IWDG->SR & 0x03);
	SET_BITS(IWDG->PR, 0, 0x05, 0x03);
	SET_BITS(IWDG->RLR, 0, 0x03E8, 0x0FFF);

	// wait for SR to be updated
	while (IWDG->SR & 0x03);

	// start the watchdog
	IWDG->KR = 0xAAAA;

	return 0;
}

//! Reset IWDG periodically
int iwdg_reset(void) {
	// write the reload register
	IWDG->KR = 0xAAAA;
	return 0;
}
