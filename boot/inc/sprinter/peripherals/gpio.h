/**
 ******************************************************************************
 * @file           : gpio.h
 * @author         : Steven Mu
 * @summary		   : General Purpose IO Header
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

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include "sprinter/core/stm32f7.h"

/**
 * @brief GPIO Structure.
 * @details (struct gpio *) casts the address behind to the address of a GPIO struct
 * Just splits what's at that address into in this case 32 bit variables.
 */
struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
#define GPIO_PORT_INIT(port) (struct gpio *)(GPIO_BASE_ADDRESS + 0x400*port)

/* GPIO Macros */
#define PIN(port, num) 		 ((((port) - 'A') << 8) | (num))  /* makes uint16_t, top 8 port ASCII-ASCII
                                                                 A (A=0, B=1..) btm 8 pin num */
#define PINNUM(pin) 		 ((pin & 0xFF))                   /* extract pin number from a pin */
#define PINPORT(pin) 		 ((pin >> 8))                     /* extract port from a pin */

enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF,		  /* INPUT = 0, OUTPUT = 1 ... by manual */
      GPIO_MODE_ANALOG};

/**
 * @brief User Functions
 */
void gpio_pinmode(uint16_t pin, uint8_t mode);
int gpio_digital_write(uint16_t pin, uint8_t value);
int gpio_digital_read(uint16_t pin);

/**
 * @brief Helper Functions
 * DO NOT USE DIRECTLY
 */
void gpio_output_config(uint16_t pin, struct gpio* GPIO);
void gpio_input_config(uint16_t pin, struct gpio* GPIO);

#endif
