#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f7.h"
#include <stdint.h>

struct flash {
	volatile uint32_t ACR, KEYR, OPTKEYR, SR, CR, OPTCR, OPTCR1;
};
#define FLASH ((struct flash*)FLASH_ADDRESS)

#endif
