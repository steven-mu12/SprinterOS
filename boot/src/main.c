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
#define VERSION        "0.0.2"
#define BUILD_DATE     "2025-12-24"
#define UART_COMM_PORT 1

#define TEST_SYSCLK    0

/** 
 * @brief SPRINTEROS BOOTLOADER ERROR STATE
 * @note type 1 means happened at sysclk set, 2 means happened at timer setup, 3 means at UART setup
 *       type 0 means anything else after we got UART (since we can now print stuff)
 */
int error(uint8_t type) {
    // blink onboard LED type amount of times for the error
    uint16_t led_pin = PIN('B', 7);
    gpio_pinmode(led_pin, GPIO_MODE_OUTPUT, 0x00);

    while (1) {
        if (type == 0) {
            while (1) {
                gpio_digital_write_sys(led_pin, 1);
                for (volatile int i=0; i< 4000000; i++);
                gpio_digital_write_sys(led_pin, 0);
                for (volatile int i=0; i< 4000000; i++);

                iwdg_reset();
            }

        } else {
            for (int j = 0; j < type; j++) {
                gpio_digital_write_sys(led_pin, 1);
                for (volatile int i=0; i< 4000000; i++);
                gpio_digital_write_sys(led_pin, 0);
                for (volatile int i=0; i< 4000000; i++);
            }
            for (volatile int i=0; i< 24000000; i++);

            // no worries about watchdog because it hasnt been set up yet
        }
    }
}

/** 
 * @brief SPRINTEROS BOOTLOADER ENTRY POINT
 */
int main(void) {

    // ============ BASIC PERIPHERAL INITS ============

    // switch the SYSCLK from HSI->PLL to use 180MHz
    if (sysclk_set_180mhz()) {
        error(1);
    }

    // initialize simple timer, then make global simple timer point to this
    // - note that this is defined at a set memory address, known by all modules in the boot
    //   that includes stm32f7.h. Since single threaded execution, no contention.
    if (init_basic_timer(TIM_6, &__global_simple_timer_ptr__)) {
        error(2);
    }

    // turn on UART output (hard coded to USART_1 for now)
    if (uart_init(UART_COMM_PORT)) {
        error(3);
    }
    uart_out("");
    uart_out("UART initialized");
    uart_out("UART Used: %d", UART_COMM_PORT);

    // Verify the clock with systick
    if (TEST_SYSCLK) {
        for (int j = 0; j < 20; j++) {
            for (int i = 0; i < 100; i++) {
                SysTick->LOAD = 1800000 - 1;
                SysTick->VAL = 0;
                SysTick->CTRL = 5;

                while ((SysTick->CTRL & (1 << 16)) == 0);
                SysTick->CTRL = 0;
            }
            uart_out("~1 second elapsed");
        } 
    }

    // introductory messsage
    uart_out("SprinterBoot v%s (BUILD %s)", VERSION, BUILD_DATE);
    
    // initialize watchdog timer
    if (iwdg_init()) {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialization Failed");
        error(0);
    } else {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialized");
        iwdg_reset();
    }

    // ============ LOAD KERNERL FROM SD CARD ============

    // initialize SPI (using SPI4 for boot)
    SPI* spi_master;
    if (init_spi(&spi_master, SPI1)) {
        uart_out("[ SPI ]: SPI Init Failed");
        error(0);
    } else {
        uart_out("[ SPI ]: SPI Init Successful");
    }

    iwdg_reset();

    // initialize the SD Card connected via SPI
    if (init_sd_slave(&spi_master, SPI1)) {
        uart_out("[ SPI ]: SD Card Init Failed");
        error(0);
    } else {
        uart_out("[ SPI ]: SD Card Init Successful");
    }

    iwdg_reset();

    /* loop forever */
    while (1) {
        // Testing. If timer is off, timer reset will not happen. SW reset occurs
        iwdg_reset();
        delay_ms(800, __global_simple_timer_ptr__);
    }
}