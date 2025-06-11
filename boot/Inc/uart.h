/**
 ******************************************************************************
 * @file           : uart.h
 * @author         : Steven Mu
 * @summary		   : UART Headers
 ******************************************************************************
 * MIT License

 * Copyright (c) 2024 Steven Mu

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 all
 * copies or substantial portions of the Software.
 ******************************************************************************
 */

#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#include "stm32f7.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/* UART structure */
struct uart {
	volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR;
};
#define UART_1 ((struct uart*)UART_1_BASE)

/* user functions */
int uart_init(int uart_id);
int uart_out(char* string, ...);

/* helper function - do not use directly */
int uart_write_char(char data);
int uart_gpio_setmode(
	uint16_t TX_PIN, uint16_t RX_PIN, uint8_t AF_ID_TX, uint8_t AF_ID_RX);
int uart_output_hex(int input);
int uart_output_int(int input);
int uart_output_str(char* input);

#endif
