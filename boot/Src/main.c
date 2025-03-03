/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Steven Mu
 * @summary		   : Main bootloader entrypoint
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
#include <stddef.h>
#include <stdarg.h>
#include "stm32f7.h"
#include "rcc.h"
#include "flash.h"
#include "gpio.h"
#include "uart.h"
#include "iwdg.h"

//#if !defined(__SOFT_FP__) && defined(__ARM_FP)
//  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
//#endif

/* CONFIGURATIONS */
#define UART_COMM_PORT		1


int main(void) {

    // switch the SYSCLK to HSI->PLL to use 180MHz
    sysclk_set_180mhz();

    // turn on UART output (hard coded to USART_1 for now)
    uart_init(UART_COMM_PORT);
    uart_out("[ UART ]: UART initialized");
    uart_out("[ UART ]: UART Used: %d", 1);

    // initialize watchdog timer
    if (iwdg_init()) {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialization Failed");
    } else {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialized");
    }

    /* loop forever */
    while (1) {
    	if (!iwdg_reset()) {
    		uart_out("[ IWDG ]: IWDG Reset Occured");
    	}
    }
}
