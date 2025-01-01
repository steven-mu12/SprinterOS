/**
 ******************************************************************************
 * @file           : gpio.h
 * @author         : Steven Mu
 * @summary		   : General Purpose IO Functionalities
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
#include "gpio.h"

/* HELPER FUNCTIONS - DO NOT USE DIRECTLY */

void gpio_output_config(uint16_t pin, struct gpio* GPIO) {

    // set the output type, speed and PUPD
    SET_BITS(GPIO->OTYPER, pin, 0x00, 0x01);		/* set output type to push-pull */
    SET_BITS(GPIO->OSPEEDR, pin*2, 0x02, 0x03);		/* set speed to second highest speed */
    SET_BITS(GPIO->PUPDR, pin*2, 0x00, 0x03);		/* don't use pu/pd because we're on push-pull */
}


void gpio_input_config(uint16_t pin, struct gpio* GPIO) {
    SET_BITS(GPIO->PUPDR, pin*2, 0x02, 0x03);		/* use pull down mode (pin=0 when hanging) */
}


/* USER FUNCTIONS */

void gpio_pinmode(uint16_t pin, uint8_t mode) {

    // create gpio structure for the io pin's port
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port); 	/* this is a pointer to the gpio structure
                                                       which we want to create where the stuff
                                                       is stored in memory */
    // enable the clock and set the mode
    RCC->AHB1ENR |= SET_BITMASK(pin_port);			/* set port clock to on - see datasheet */
    SET_BITS(GPIO->MODER, pin_num*2, mode, 0x03);	/* set the pin to desired mode */

    // specific pin configs depending on mode
    switch(mode) {
        case GPIO_MODE_INPUT:
            gpio_output_config(pin, GPIO);
            break;
        case GPIO_MODE_OUTPUT:
            gpio_input_config(pin, GPIO);
            break;
        case GPIO_MODE_AF:
            return;									/* way too specific depending on peripheral to do here */
        case GPIO_MODE_ANALOG:
            return;									/* later implementation */
    }
    return;
}


int gpio_digital_write(uint16_t pin, uint8_t value) {

    // access pin registers via gpio struct
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port);

    // write to BSRR
    if (value == 1) {
        GPIO->BSRR = SET_BITMASK(pin_num);			/* write to set register (lower 16 of 32 bits) */
    } else {
        GPIO->BSRR = SET_BITMASK((pin_num + 16));	/* write to reset register (higher 16 of 32 bits) */
    }

    // check if ODR reflects the changes
    for (int i=0; i<5; i++);						/* tiny 5 clock cycle delay */

    if (READ_BIT(GPIO->ODR, pin_num) != value) {	/* check ODR register */
        return 1;
    } else {
        return 0;
    }
}


int gpio_digital_read(uint16_t pin) {

    // access pin registers via gpio struct
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port);

    // read the pin and return value
    return (READ_BIT(GPIO->IDR, pin_num));
}
