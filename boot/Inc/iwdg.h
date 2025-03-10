/**
 ******************************************************************************
 * @file           : iwdg.h
 * @author         : Steven Mu
 * @summary		   : Independent Watchdog Timer Header
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
#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>
#include "stm32f7.h"
#include "rcc.h"

/* IWDG structure */
struct iwdg {
	volatile uint32_t KR, PR, RLR, SR, WINR;
};
#define IWDG ((struct iwdg *) IWDG_BASE)

/* User Functions */
int iwdg_init(void);
int iwdg_reset(void);
//int iwdg_bypass(void);

/* Helper Functions - DO NOT USE DIRECTLY */

#endif
