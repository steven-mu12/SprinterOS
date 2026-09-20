#ifndef __NVIC_COMMON_H__
#define __NVIC_COMMON_H__

#include <stdint.h>

#include "stm32f7.h"

struct nvic {
    volatile uint32_t ISER[8];
    const    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
    const    uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];
    const    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];
    const    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];
    const    uint32_t RESERVED4[56];
    volatile uint8_t  IPR[240];
};
#define NVIC_REGS ((struct nvic *) NVIC_ADDRESS)

/* irq numbers (RM0410 table 46), add as drivers need them */
#define TIM6_DAC_IRQ                    54
#define TIM7_IRQ                        55

#endif
