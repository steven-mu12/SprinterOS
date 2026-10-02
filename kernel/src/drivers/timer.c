#include <assert.h>
#include <inttypes.h>
#include <stdint.h>

#include "drivers/timer.h"

#include "nvic.h"
#include "stm32f7.h"
#include "core/sprinter_common.h"
#include "drivers/rcc.h"

int init_basic_timer(TIM_NUM const timer_pin, BASIC_TIM** timer, int is_sysclk) {
    if (timer == NULL) {
        return _ERR;
    }

    /* depending which timer, enable in RCC if we need to */
    if (timer_pin == TIM_6) {
        SET_BIT(RCC->APB1ENR, 4);
    } else if (timer_pin == TIM_7) {
        SET_BIT(RCC->APB1ENR, 5);
    } else {
        return 1;
    }

    *timer = (BASIC_TIM *)(BASIC_TIM_BASE + 0x400 * timer_pin);

    /* set up register */
    if (is_sysclk) {
        RESET_BIT((*timer)->CR1, 3);  /* set one pulse mode off */
    } else {
        SET_BIT((*timer)->CR1, 3);  /* set one pulse mode on */
    }

    SET_BIT((*timer)->CR1, 2);  /* set URS to only counter overflow generates interrupt*/
    SET_BITS((*timer)->CR2, 4, 0x02, 0x0F);  /* set MMS to use overflow as update event */

    /* Load the prescaler and generate event */
    SET_BITS((*timer)->PSC, 0, BASIC_TIM_PSC, 0xFFFF);
    SET_BIT((*timer)->EGR, 0);

    return 0;
}

int delay_ms(uint16_t ms, BASIC_TIM* timer) {
    if (timer == NULL) {
        return _ERR;
    }

    int remaining_ms = ms;
    int current_interval_ms;
    while (remaining_ms > 0) {
        /* if more than max, then pass in only max */
        if (remaining_ms > MAX_INTERVAL_MS) {
            current_interval_ms = MAX_INTERVAL_MS;
            remaining_ms -= MAX_INTERVAL_MS;
        } else {
            current_interval_ms = remaining_ms;
            remaining_ms = 0;
        }

        /* clear SR if leftover from previous run, and run */
        RESET_BIT(timer->SR, 0);
        SET_BITS(timer->ARR, 0, (current_interval_ms * 10) - 1, 0xFFFF);
        SET_BIT(timer->EGR, 0);
        SET_BIT(timer->CR1, 0);

        while (READ_BIT(timer->SR, 0) != 1);
    }

    return 0;
}

/* TIM7 used as system clock */
TIM_NUM const system_timer_id = TIM_7;

typedef struct SYSTEM_CLOCK {
    BASIC_TIM* sysclk;
    volatile uint32_t iteration;
    /* 4d 23h 18m 16.7s */
} SYSTEM_CLOCK;

/* system clock */
SYSTEM_CLOCK system_clock;

int start_system_clock(void) {
    system_clock.iteration = 0;
    if (init_basic_timer(system_timer_id, &system_clock.sysclk, 1)) {
        return _NOP;
    }

    /* explicitly set ARR to 0xFFFF for beginning */
    RESET_BIT(system_clock.sysclk->SR, 0);
    SET_BITS(system_clock.sysclk->ARR, 0, 0xFFFF, 0xFFFF);
    SET_BIT(system_clock.sysclk->EGR, 0);

    /* since system timer rn is just for timestamps and is so simple 10 should be okay */
    SET_BITS(NVIC_REGS->IPR[TIM7_IRQ], 0, (uint8_t)SHPR_PRIORITY(10), 0xFF);

    SET_BIT(system_clock.sysclk->DIER, 0); /* enable system interrupt */
    NVIC_REGS->ISER[TIM7_IRQ / 32] = SET_BITMASK(TIM7_IRQ % 32);
    SET_BIT(system_clock.sysclk->CR1, 0);

    return 0;
}

/* reset, fires every time the timer expires */
void TIM7_IRQHandler(void) {
    RESET_BIT(system_clock.sysclk->SR, 0);

    (void)system_clock.sysclk->SR;

    system_clock.iteration++;
}

void read_system_clock(char* sys_timestamp, size_t sys_timestamp_size) {
    uint32_t ticks = (system_clock.iteration * (0xFFFF + 1)) + system_clock.sysclk->CNT;
    snprintf(sys_timestamp, sys_timestamp_size, "%lu.%04lu", ticks / 10000, ticks % 10000);
}
