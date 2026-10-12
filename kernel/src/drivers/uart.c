#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#include "drivers/uart.h"
#include "core/scheduler.h"
#include "cortex_m7.h"
#include "nvic.h"
#include "stm32f7.h"

#define UART_RX_PRIORITY 4

static int uart_write_char(char data) {
	while (READ_BIT(UART_1->ISR, 7) == 0);			/* check TXE till high */
	UART_1->TDR = (data & 0xFF);					/* write data into TDR */
	while (READ_BIT(UART_1->ISR, 7) == 0);			/* check TXE & TC to be high */

	return 0;
}

/**
 * support various output formats
 */
static int uart_output_hex(int input) {
	char hex_char = 0;
	uint32_t value = 0;

	/* output the hex 0x */
	uart_write_char('0');
	uart_write_char('x');

	/* repeatedly mask for 4 bits at a time */
	for (int i = 28; i >= 0; i-=4) {
		value = (input >> i) & 0x0F;				/* get value of the 4 bits we're on */
		if (value < 10) {							/* depends on value, print hex char */
			hex_char = '0' + value;
		} else {
			hex_char = 'A' + (value - 10);
		}
		uart_write_char(hex_char);
	}

	return 0;
}

static int uart_output_int(int input) {
	char char_buffer[12];
	snprintf(char_buffer, sizeof(char_buffer), "%d", input);

	for (int i = 0; char_buffer[i] != '\0'; i++) {
		uart_write_char(char_buffer[i]);
	}

	return 0;
}

int uart_output_str(char* input) {
	while (*input != '\0') {
		uart_write_char(*input);
		input++;
	}

	return 0;
}

/*
 * interrupt handler for RX
 */
static struct uart_rx_buf rx_buf;

void USART1_IRQHandler(void) {
	char character = (char)READ_BITS(UART_1->RDR, 0, 0xFF);
	rx_buf.rx_buffer[rx_buf.tail] = character;
	rx_buf.latest_char = character;
	
	/* overwrite oldest if full */
	rx_buf.tail = (rx_buf.tail + 1) % UART_RX_BUFFER_SIZE;
    if (rx_buf.tail == rx_buf.head) {
        rx_buf.head = (rx_buf.head + 1) % UART_RX_BUFFER_SIZE;
    }

	/* we should never actually see this overrun unless something is higher prio */
	if (READ_BIT(UART_1->ISR, 3)) {
		SET_BIT(UART_1->ICR, 3);
	}

	/* wake the shell task (use address of rx_buf as key) */
	sched_wake(&rx_buf);
}

/**
 * User Functions
 */
int uart_out(char* string, ...) {
	va_list args;
	va_start(args, string);

	if (string == NULL) {
		return 1;
	}

	while (*string != '\0') {
		if (*string == '%') {
			string++;
			if (*string == 'h') {
				uart_output_hex(va_arg(args, int));
			} else if (*string == 'd') {
				uart_output_int(va_arg(args, int));
			} else if (*string == 's') {
				uart_output_str(va_arg(args, char*));
			}
			else {
				break;
			}
		} else {
			uart_write_char(*string);			/* output the first char string is pointing to */
		}

		string++;								/* increment character pointer by sizeof(char) */
	}

	/* resolve newline and return carriage chars */
	char new_line = '\r';
	uart_write_char(new_line);
	new_line = '\n';
	uart_write_char(new_line);

	/* to indicate end of transmission, TC bit is pulled high. Poll until done */
	while ((READ_BIT(UART_1->ISR, 6) == 0));

	return 0;
}

int uart_output_char(char data) {
	return uart_write_char(data);
}

int uart_rx_init(void) {
    SET_BIT(UART_1->CR1, 5);

    NVIC_REGS->IPR[USART1_IRQ] = (uint8_t)IRQ_PRIORITY(UART_RX_PRIORITY);
    NVIC_REGS->ISER[USART1_IRQ / 32] = SET_BITMASK(USART1_IRQ % 32);

    return 0;
}

char uart_read(void) {
    char data;

    irq_disable();
		while (rx_buf.head == rx_buf.tail) {
			sched_block(&rx_buf);
		}
		data = rx_buf.rx_buffer[rx_buf.head];
		rx_buf.head = (rx_buf.head + 1) % UART_RX_BUFFER_SIZE;
    irq_enable();

    return data;
}
