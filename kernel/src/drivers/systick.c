#include <stdint.h>

#include "drivers/systick.h"

#include "cortex_m7.h"
#include "stm32f7.h"
#include "drivers/timer.h"
#include "drivers/uart.h"

/* the f7 holds priority in the top nibble of the byte, 0 highest, 15 lowest */
#define SHPR_PRIORITY(PRIO)     ((PRIO) << 4)
#define SHPR3_PENDSV            16
#define SHPR3_SYSTICK           24

/* CTRL bits, clksource left low for the ahb / 8 reference */
#define SYSTICK_CTRL_ENABLE     0
#define SYSTICK_CTRL_TICKINT    1

static volatile uint32_t systick_count;

int systick_setup(void) {
    /* pendsv and systick low prio */
    SET_BITS(SCB_REGS->SHPR3, SHPR3_PENDSV, SHPR_PRIORITY(15), 0xFF);
    SET_BITS(SCB_REGS->SHPR3, SHPR3_SYSTICK, SHPR_PRIORITY(14), 0xFF);

    SYSTICK_REGS->LOAD = SYSTICK_RELOAD_MS(SYSTICK_TIMESLICE_MS);
    SYSTICK_REGS->VAL = 0;

    SYSTICK_REGS->CTRL = SET_BITMASK(SYSTICK_CTRL_TICKINT) | SET_BITMASK(SYSTICK_CTRL_ENABLE); /* count */

    return 0;
}

void SysTick_Handler(void) {
    /* do thing rn but have pendsv set here later */
}
