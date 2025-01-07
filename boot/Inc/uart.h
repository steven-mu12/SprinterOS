#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include "stm32f7.h"
#include "gpio.h"

/* UART structure */
struct uart {
	volatile uint32_t CR1, CR2, CR3, BRR, RTOR, RQR, ISR, ICR, RDR, TDR;
};
#define UART_1 ((struct uart *) UART_1_BASE)

/* user functions */
int uart_init(int uart_id);
int uart_out(char* string);

/* helper function - do not use directly */
int uart_write_char(char data);

#endif
