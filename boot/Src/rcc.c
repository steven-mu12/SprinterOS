#include <stdint.h>
#include "stm32f7.h"
#include "rcc.h"
#include "flash.h"

int sysclk_set_216mhz(void) {
	// ensure that the HSI enabled and running
	while ((RCC->CR & 0x01) == 0);					/* check HSION */
	while ((RCC->CR & 0x02) == 0);					/* check HSIRDY */

	// setup PLL configurations
	RCC->PLLCFGR = 0;								/* reset the PLL config reg */
	RCC->PLLCFGR &= RESET_BITMASK(22);				/* set PLL source to HSI */
	SET_BITS(RCC->PLLCFGR, 0, 0x10, 0x3F);			/* set PLLM (slice up 16MHz to 1MHz) */
	SET_BITS(RCC->PLLCFGR, 6, 0x1B0, 0x1FF);			/* set PLLN (multiplies by 432) */
	SET_BITS(RCC->PLLCFGR, 16, 0x00, 0x03);			/* set PLLP (divides by 2) = 216MHz */

	// turn on the PLL and wait for stable
	RCC->CR |= SET_BITMASK(24);
	while (READ_BIT(RCC->CR, 25) == 0);

	// set flash to respond to new clock rate
	SET_BITS(FLASH->ACR, 0, 0x07, 0x0F);				/* 7 wait states for 216MHz */

	// set bus prescaler
	SET_BITS(RCC->CFGR, 4, 0x00, 0x0F);				/* AHB bus doesn't need prescaling, factor of 1 */
	SET_BITS(RCC->CFGR, 10, 0x06, 0x07);				/* APB1 has max of 48MHz, scale down by factor of 8 vs AHB */
	SET_BITS(RCC->CFGR, 13, 0x05, 0x07);				/* APB2 has max of 90MHz, scale down by factor of 4 */

	// switch the sysclk to be the PLL output
	SET_BITS(RCC->CFGR, 0, 0x02, 0x0F);				/* set the PLL as SYSCLK */
	while (((RCC->CFGR & 0x0C) >> 2) != 0x02);		/* wait until SWS reflects change */

	return 0;
}