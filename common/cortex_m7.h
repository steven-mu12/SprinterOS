#ifndef __CORTEX_M7_COMMON_H__
#define __CORTEX_M7_COMMON_H__

#include <stdint.h>

#include "stm32f7.h"

/*
 * SysTick core peripheral
 */
struct systick {
    volatile uint32_t CTRL, LOAD, VAL;
    const    uint32_t CALIB;
};
#define SYSTICK_REGS ((struct systick *) SYSTICK_ADDRESS)

/*
 * SCB
 */
struct scb {
    const    uint32_t CPUID;
    volatile uint32_t ICSR, VTOR, AIRCR, SCR, CCR;
    volatile uint32_t SHPR1, SHPR2, SHPR3;
    volatile uint32_t SHCSR, CFSR, HFSR, DFSR, MMFAR, BFAR, AFSR;
};
#define SCB_REGS ((struct scb *) SCB_ADDRESS)

/*
 * disable / enable interrupts in critical sections
 */
static inline void irq_disable(void) {
    __asm volatile ("cpsid i" ::: "memory");
}
static inline void irq_enable(void) {
    __asm volatile ("cpsie i" ::: "memory");
}

int systick_setup(void);
void SysTick_Handler(void);

#endif
