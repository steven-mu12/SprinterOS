#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include "sprinter/core/stm32f7.h"

/**
 * @brief Constant values
 */
#define BASIC_TIM_PSC    0x2327        /* PSC set to 9000 - 1 = 8999, 0.1ms at (45MHz * 2) 
                                          APB1 clock rate, where prescaler used != 1 */
#define MAX_INTERVAL_MS  (0xFFFF / 0x0A)  /* divided by 10, because PSC set to 8999, which is 
                                          1/10 of a ms */

/* Enum for basic timers */
typedef enum {
    TIM_6 = 0,
    TIM_7 = 1,
} TIM_NUM;

/**
 * @brief BASIC TIMER (TIM6 and TIM7) Structure
 * @note these are some super weird mappings. Note reserved regs
 */
typedef struct {
    volatile uint32_t CR1, CR2;
    const    uint32_t RES_0;
    volatile uint32_t DIER, SR, EGR;
    const    uint32_t RES_1[3];
    volatile uint32_t CNT, PSC, ARR;
} BASIC_TIM;

/**
 * @brief Peripheral Functions
 */
int init_basic_timer(TIM_NUM const timer_pin, BASIC_TIM** timer);

/**
 * @brief Delay Module User Functions
 */
void delay_ms(uint16_t ms, BASIC_TIM* timer);  // Delay in ms

#endif
