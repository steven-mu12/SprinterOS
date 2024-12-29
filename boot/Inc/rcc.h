#ifndef __RCC_H__
#define __RCC_H__

#include <stdint.h>
#include "stm32f7.h"

/* RCC structure */
struct rcc {
  volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
      RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
      RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
      AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, RESERVED5[2], BDCR, CSR,
      RESERVED6[2], SSCGR, PLLI2SCFGR;
};
#define RCC ((struct rcc *) RCC_ADDRESS)			/* this is the location of RCC, with all registers being 32 bits */
												/* when you call any registers, it'll auto add 0x04 to offset */

/* helper functions */
int sysclk_set_216mhz(void);					/* set system clock to 84MHz via PLL */
int sysclk_test(void);							/* system clock speed test */

#endif