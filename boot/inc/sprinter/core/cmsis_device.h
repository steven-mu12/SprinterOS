#pragma once

/* STM32F767 specifics */
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

    /* later can add peripheral IRQs later if needed */
} IRQn_Type;
