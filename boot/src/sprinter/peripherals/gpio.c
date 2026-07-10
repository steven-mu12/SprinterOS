#include <stdbool.h>
#include <stdint.h>

#include "sprinter/peripherals/gpio.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals/rcc.h"
#include "sprinter/peripherals/timer.h"


static void gpio_output_config(uint16_t pin, struct gpio* GPIO) {
    SET_BITS(GPIO->OTYPER, PINNUM(pin), 0x00, 0x01);		/* set output type to push-pull */
    SET_BITS(GPIO->OSPEEDR, PINNUM(pin)*2, 0x03, 0x03);		/* set speed to the highest speed */
    SET_BITS(GPIO->PUPDR, PINNUM(pin)*2, 0x00, 0x03);		/* don't use pu/pd bc we're on push-pull */
}

static void gpio_input_config(uint16_t pin, struct gpio* GPIO) {
    SET_BITS(GPIO->PUPDR, PINNUM(pin)*2, 0x02, 0x03);		/* use pull down mode (pin=0 when hanging) */
}

static void gpio_af_config(uint16_t pin, uint8_t AF_ID, struct gpio* GPIO) {
    /* based on pin, either the high AF register is used or the low */
	bool use_high_afr = 0;
    uint8_t afr_offset = 0;

    if (PINNUM(pin) >= 8) {
		use_high_afr = 1;
		afr_offset = (PINNUM(pin) - 8) * 4;
	} else {
		use_high_afr = 0;
		afr_offset = PINNUM(pin) * 4;
	}

    /* set speed and AF */
    SET_BITS(GPIO->AFR[use_high_afr], afr_offset, AF_ID, 0x0F);
    SET_BITS(GPIO->OSPEEDR, PINNUM(pin) * 2, 0x02, 0x03);

    /* NOTE THAT THE USER SHOULD IMPLEMENT THEIR OWN SPECIFICS FOR PULL UP PULL DOWN, for idle state */
}

/**
 * User functions
 */
int gpio_pinmode(uint16_t pin, GPIO_MODE mode, uint8_t AF_ID) {
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port); 	/* this is a pointer to the gpio structure
                                                       which we want to create where the stuff
                                                       is stored in memory */
    RCC->AHB1ENR |= SET_BITMASK(pin_port);			/* set port clock to on - see datasheet */
    SET_BITS(GPIO->MODER, pin_num*2, mode, 0x03);	/* set the pin to desired mode */

    /* specific pin configs depending on mode */
    switch(mode) {
        case GPIO_MODE_INPUT:
            gpio_input_config(pin, GPIO);
            break;
        case GPIO_MODE_OUTPUT:
            gpio_output_config(pin, GPIO);
            break;
        case GPIO_MODE_AF:
            if (AF_ID == NULL) {
                return 1;
            }
            gpio_af_config(pin, AF_ID, GPIO);
            break;
        case GPIO_MODE_ANALOG:
            return 0;									/* later implementation */
    }
    return 0;
}

int gpio_digital_write(uint16_t pin, bool value) {
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port);

    /* write to BSRR based on whether we want to set or reset a bit */
    if (value) {
        GPIO->BSRR = SET_BITMASK(pin_num);			/* write to set register (lower 16b) */
    } else {
        GPIO->BSRR = SET_BITMASK((pin_num + 16));	/* write to reset register (higher 16b) */
    }

    /* check if ODR reflects the changes after a tiny delay */
    delay_ms(5, __global_simple_timer_ptr__);
    if (READ_BIT(GPIO->ODR, pin_num) != value) {
        return 1;
    } else {
        return 0;
    }
}

/* this is to be used if GPIO needs to be written before timer periperal is set up */
int gpio_digital_write_sys(uint16_t pin, bool value) {
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port);

    if (value) {
        GPIO->BSRR = SET_BITMASK(pin_num);
    } else {
        GPIO->BSRR = SET_BITMASK((pin_num + 16));
    }

    // the difference from the user version, is this uses a fake polled delay
    for (volatile int i=0; i<1000; i++);

    if (READ_BIT(GPIO->ODR, pin_num) != value) {
        return 1;
    } else {
        return 0;
    }
}

int gpio_digital_read(uint16_t pin) {
    uint8_t pin_num = PINNUM(pin);
    uint8_t pin_port = PINPORT(pin);
    struct gpio *GPIO = GPIO_PORT_INIT(pin_port);

    return (READ_BIT(GPIO->IDR, pin_num));
}
