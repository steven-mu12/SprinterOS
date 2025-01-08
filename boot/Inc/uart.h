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
int uart_gpio_setmode(uint16_t TX_PIN, uint16_t RX_PIN, uint8_t AF_ID_TX, uint8_t AF_ID_RX);

#endif
