#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdbool.h>
#include <stdint.h>

#include "sprinter/core/stm32f7.h"

/**
 * GPIO Helpful Macros
 */
#define PIN(port, num) 		 ((((port) - 'A') << 8) | (num))  /* makes uint16_t, top 8 port ASCII-ASCII
                                                                 A (A=0, B=1..) btm 8 pin num */
#define PINNUM(pin) 		 ((pin & 0xFF))                   /* extract pin number from a pin */
#define PINPORT(pin) 		 ((pin >> 8))                     /* extract port from a pin */

/**
 * GPIO structure
 * move to first register in port's address range and treat as following registers
 */
struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
#define GPIO_PORT_INIT(port) (struct gpio *)(GPIO_BASE_ADDRESS + 0x400*port)

/**
 * GPIO Mode Enum
 * this is the standard according to documentation
 */
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_AF = 2,
    GPIO_MODE_ANALOG = 3,
} GPIO_MODE;

/**
 * User Functions
 */
int gpio_pinmode(uint16_t pin, GPIO_MODE mode, uint8_t AF_ID);
int gpio_digital_write(uint16_t pin, bool value);
int gpio_digital_read(uint16_t pin);
int gpio_digital_write_sys(uint16_t pin, bool value);

#endif
