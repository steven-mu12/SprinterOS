#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "stm32f7.h"

#define UART_RX_BUFFER_SIZE		1024

/* uart mmio register structure */
struct uart {
	volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR;
};
#define UART_1 ((struct uart *) UART_1_BASE)

struct uart_rx_buf {
	char rx_buffer[UART_RX_BUFFER_SIZE];
	volatile uint32_t head;
	volatile uint32_t tail;
	char latest_char;
};

/* user functions*/
int uart_rx_init(void);
int uart_out(char* string, ...);
int uart_output_char(char data);
int uart_output_str(char* input);
char uart_read(void);

#endif
