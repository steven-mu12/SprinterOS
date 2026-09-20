#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>

#include "rcc.h"

#define SYSTICK_CLK_HZ          (SYSCLK_HZ / 8)
#define SYSTICK_RELOAD_MS(MS)   ((((SYSTICK_CLK_HZ) / 1000) * (MS)) - 1)
#define SYSTICK_TIMESLICE_MS    100

int systick_setup(void);

#endif /* __SYSTICK_H__ */
