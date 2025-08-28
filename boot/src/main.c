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

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals.h"

/** 
 * @brief MAIN BOOT EXECUTIVE CONFIGURATIONS 
 */
// TODO (smu): Migrate this to a config file
#define VERSION       "0.0.1"
#define BUILD_DATE    "2025-08-18"
#define UART_COMM_PORT 1

/** 
 * @brief SPRINTEROS BOOTLOADER ENTRY POINT
 */
int main(void) {
    // switch the SYSCLK from HSI->PLL to use 180MHz
    sysclk_set_180mhz();

    // turn on UART output (hard coded to USART_1 for now)
    uart_init(UART_COMM_PORT);

    uart_out("");
    uart_out("UART initialized");
    uart_out("UART Used: %d", UART_COMM_PORT);

    // introductory messsage
    uart_out("SprinterBoot v%s (BUILD %s)", VERSION, BUILD_DATE);

    // initialize watchdog timer
    if (iwdg_init()) {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialization Failed");
    } else {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialized");
    }

    // initialize timer
    BASIC_TIM* timer;
    if (init_basic_timer(TIM_6, &timer)) {
        uart_out("[ TIMER ]: Timer initialization Failed");
    } else {
        uart_out("[ TIMER ]: Timer initialized");
    }

    /* loop forever */
    while (1) {
        // Testing. If timer is off, timer reset will not happen. SW reset occurs
        iwdg_reset();
        delay_ms(800, timer);
    }
}
