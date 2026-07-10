#pragma once

/**
 * ARM CMSIS (Cortex Microcontroller Software Interface Standard)
 * 
 * this is a stripped down version of stm32f767's core_cm7.h headers, since 
 * this is a minimal boot environment
 */

#define __NVIC_PRIO_BITS 4
#define __Vendor_SysTickConfig 0

typedef enum IRQn {
    NonMaskableInt_IRQn   = -14,
    HardFault_IRQn        = -13,
    MemoryManagement_IRQn = -12,
    BusFault_IRQn         = -11,
    UsageFault_IRQn       = -10,
    SVCall_IRQn           = -5,
    DebugMonitor_IRQn     = -4,
    PendSV_IRQn           = -2,
    SysTick_IRQn          = -1,

    /* can add peripheral IRQs later if needed */
} IRQn_Type;
