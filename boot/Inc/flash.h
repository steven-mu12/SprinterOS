#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>
#include "stm32f7.h"

struct flash {
    volatile uint32_t ACR, KEYR, OPTKEYR, SR, CR, OPTCR, OPTCR1;
};
#define FLASH ((struct flash *) FLASH_ADDRESS)

#endif
