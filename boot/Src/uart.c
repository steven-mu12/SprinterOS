/**
 ******************************************************************************
 * @file           : gpio.c
 * @author         : Steven Mu
 * @summary		   : UART Functionalities
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

#include "uart.h"
#include "flash.h"
#include "gpio.h"
#include "rcc.h"
#include "stm32f7.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* HELPER FUNCTIONS - do not use directly */

int uart_write_char(char data)
{

	while (READ_BIT(UART_1->ISR, 7) == 0)
		;						 /* check TXE till high */
	UART_1->TDR = (data & 0xFF); /* write data into TDR */
	while (READ_BIT(UART_1->ISR, 7) == 0)
		; /* check TXE & TC to be high */

	return 0;
}

int uart_output_hex(int input)
{

	char hex_char = 0;
	uint32_t value = 0;

	// output the hex 0x
	uart_write_char('0');
	uart_write_char('x');

	// mask for the first 4 bits, then next, then next
	for (int i = 28; i >= 0; i -= 4) {
		value = (input >> i) & 0x0F; /* get the value of the 4 bits we're on */

		if (value < 10) { /* depending on value, print hex char */
			hex_char = '0' + value;
		} else {
			hex_char = 'A' + (value - 10);
		}
		uart_write_char(hex_char);
	}

	return 0;
}

int uart_output_int(int input)
{
	char char_buffer[12];
	snprintf(char_buffer, sizeof(char_buffer), "%d", input);

	for (int i = 0; char_buffer[i] != '\0'; i++) {
		uart_write_char(char_buffer[i]);
	}

	return 0;
}

int uart_output_str(char* input)
{
	while (*input != '\0') {
		uart_write_char(*input);
		input++;
	}

	return 0;
}

int uart_gpio_setmode(
	uint16_t TX_PIN, uint16_t RX_PIN, uint8_t AF_ID_TX, uint8_t AF_ID_RX)
{

	// set up the GPIO pins themselves
	struct gpio* TX_GPIO = GPIO_PORT_INIT(PINPORT(TX_PIN));
	struct gpio* RX_GPIO = GPIO_PORT_INIT(PINPORT(RX_PIN));

	// configure the pins to AF mode
	gpio_pinmode(TX_PIN, GPIO_MODE_AF);
	gpio_pinmode(RX_PIN, GPIO_MODE_AF);

	// determine if AFR high/low needs to be used
	uint8_t target_afr_tx = 0, target_bit_tx = 0, target_afr_rx = 0,
			target_bit_rx = 0;

	if (PINNUM(TX_PIN) >= 8) {
		target_afr_tx = 1;
		target_bit_tx = (PINNUM(TX_PIN) - 8) * 4;
	} else {
		target_afr_tx = 0;
		target_bit_tx = PINNUM(TX_PIN) * 4;
	}

	if (PINNUM(RX_PIN) >= 8) {
		target_afr_rx = 1;
		target_bit_rx = (PINNUM(RX_PIN) - 8) * 4;
	} else {
		target_afr_rx = 0;
		target_bit_rx = PINNUM(RX_PIN) * 4;
	}

	// set both TX and RX to their AF IDs
	SET_BITS(TX_GPIO->AFR[target_afr_tx], target_bit_tx, AF_ID_TX, 0x0F);
	SET_BITS(RX_GPIO->AFR[target_afr_rx], target_bit_rx, AF_ID_RX, 0x0F);

	// set output speed for TX and RX
	SET_BITS(TX_GPIO->OSPEEDR, PINNUM(TX_PIN) * 2, 0x02, 0x03);
	SET_BITS(RX_GPIO->OSPEEDR, PINNUM(RX_PIN) * 2, 0x02, 0x03);

	// use pull up for UART lines on idle (because of how UART communication
	// works)
	SET_BITS(TX_GPIO->PUPDR, PINNUM(TX_PIN) * 2, 0x01, 0x03);
	SET_BITS(RX_GPIO->PUPDR, PINNUM(RX_PIN) * 2, 0x01, 0x03);

	return 0;
}

/* USER FUNCTIONS */

int uart_init(int uart_id)
{

	// only support for UART 1
	if (uart_id == 1) {
		RCC->APB2ENR |= SET_BITMASK(4);
	}

	// configure TX/RX settings
	if (READ_BIT(UART_1->CR1, 0) == 1) { /* turn off UART if it's on */
		UART_1->CR1 |= SET_BITMASK(0);
	}

	SET_BITS(UART_1->CR1, 28, 0x00, 0x01); /* set word size to 8 bit */
	SET_BITS(UART_1->CR1, 12, 0x00, 0x01);
	SET_BITS(UART_1->CR2, 12, 0x00, 0x03); /* set stop bit to 1 */
	SET_BITS(UART_1->CR1, 10, 0x00, 0x01); /* set parity to none */

	// set the baud rate (more support coming)
	SET_BITS(UART_1->BRR, 0, 0x30D, 0x3FF); /* set BRR to 781 for 115200 baud */

	// enable the UART port
	SET_BITS(UART_1->CR1, 0, 0x01, 0x01); /* turn on UART */
	SET_BITS(UART_1->CR1, 2, 0x01, 0x01); /* turn on RX */
	SET_BITS(UART_1->CR1, 3, 0x01, 0x01); /* turn on TX */

	// set the UART pins as ALT FUNCTION
	static uint16_t TX_PIN = PIN('A', 9);
	static uint16_t RX_PIN = PIN('A', 10);

	if (uart_gpio_setmode(TX_PIN, RX_PIN, 0x07, 0x07) == 1) {
		return 1;
	}

	return 0;
}

int uart_out(char* string, ...)
{

	// set up argument list
	va_list args;
	va_start(args, string);

	if (string == NULL) {
		return 1;
	}

	// break up the string one by one
	while (*string != '\0') {

		// catch the replacements
		if (*string == '%') {
			string++;
			if (*string == 'h') {
				uart_output_hex(va_arg(args, int));
			} else if (*string == 'd') {
				uart_output_int(va_arg(args, int));
			} else if (*string == 's') {
				uart_output_str(va_arg(args, char*));
			} else {
				break;
			}
		} else {
			uart_write_char(
				*string); /* output the first char string is pointing to */
		}

		string++; /* increment character pointer by sizeof(char) */
	}

	// write the NEWL and CARRIAGE RET character
	char new_line = '\r';
	uart_write_char(new_line);
	new_line = '\n';
	uart_write_char(new_line);

	// wait for TC to get pulled high
	while ((READ_BIT(UART_1->ISR, 6) == 0))
		;

	return 0;
}
