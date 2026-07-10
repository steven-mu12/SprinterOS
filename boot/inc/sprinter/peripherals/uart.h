#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "sprinter/core/stm32f7.h"

/* uart mmio register structure */
struct uart {
	volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR;
};
#define UART_1 ((struct uart *) UART_1_BASE)

/* user functions*/
int uart_init(int uart_id);
int uart_out(char* string, ...);

#endif
