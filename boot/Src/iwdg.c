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
#include "stm32f7.h"
#include "rcc.h"
#include "iwdg.h"


/* USER FUNCTIONS */

int iwdg_init(void) {

	// init the LSI oscillator and wait for stable
	SET_BITS(RCC->CSR, 0, 0x01, 0x01);
	while (!READ_BIT(RCC->CSR, 1));

	// init the IWDG by writing special code to KR
	SET_BITS(IWDG->KR, 0, 0xCCCC, 0xFFFF);
	SET_BITS(IWDG->KR, 0, 0x5555, 0xFFFF);

	// set the prescaler to get ~ a 500ms expiration
	while (READ_BIT(IWDG->SR, 1));
	if (READ_BIT(IWDG->SR, 0)) {
		SET_BITS(IWDG->PR, 0, 0x03, 0x07);
	}

	// write the reload register
	while (READ_BIT(IWDG->SR, 1));
	if (READ_BIT(IWDG->SR, 1) == 0) {
		SET_BITS(IWDG->RLR, 0, 0x3E8, 0x0FFF);

		// wait for SR to be updated
		while (READ_BITS(IWDG->SR, 0, 0xFFFFFFFF) != 0x00000000);
	}

	// start the watchdog
	SET_BITS(IWDG->KR, 0, 0xAAAA, 0xFFFF);

	return 0;
}


int iwdg_reset(void) {

	// write the reload register
	while (READ_BIT(IWDG->SR, 1));
	if (READ_BIT(IWDG->SR, 1) == 0) {
		SET_BITS(IWDG->RLR, 0, 0x3E8, 0x0FFF);

		// wait for SR to be updated
		while (READ_BITS(IWDG->SR, 0, 0xFFFFFFFF) != 0x00000000);
	}

	return 0;
}
