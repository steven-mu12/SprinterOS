#include <stdint.h>

#include "sprinter/peripherals/iwdg.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals/rcc.h"

int iwdg_init(void) {
	/* init the LSI oscillator and wait for stable */
	SET_BITS(RCC->CSR, 0, 0x01, 0x01);
	while (!READ_BIT(RCC->CSR, 1));

	/* init the IWDG by writing special code to KR */
	IWDG->KR = 0xCCCC;
	IWDG->KR = 0x5555;

	/* prescale to be around 1000ms expiration */
	while (IWDG->SR & 0x03);
	SET_BITS(IWDG->PR, 0, 0x05, 0x03);
	SET_BITS(IWDG->RLR, 0, 0x03E8, 0x0FFF);
	while (IWDG->SR & 0x03);

	/* load and start the watchdog */
	IWDG->KR = 0xAAAA;

	return 0;
}

int iwdg_reset(void) {
	IWDG->KR = 0xAAAA;
	return 0;
}
