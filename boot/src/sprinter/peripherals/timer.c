#include <assert.h>
#include <stdint.h>

#include "sprinter/peripherals/timer.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals/rcc.h"

/**
 * @brief Initialize basic timer
 * @return Error code
 */
int init_basic_timer(
    TIM_NUM const timer_pin,  //! [in] Timer PIN we want to use
    BASIC_TIM** timer          //! [in] Timer object to set up
) {
    assert(timer != NULL);

    // Enable the timer clock in RCC, based on which pin is used
    if (timer_pin == TIM_6) {
        SET_BIT(RCC->APB1ENR, 4);
    } else if (timer_pin == TIM_7) {
        SET_BIT(RCC->APB1ENR, 5);
    } else {
        return 1;
    }

    // Casting to pointer -> telling C to treat address represented by an integer as a 
    // type BASIC_TIM object. Hence why we can't just directly cast to BASIC_TIM
    *timer = (BASIC_TIM *)(BASIC_TIM_BASE + 0x400 * timer_pin);

    // Initial register setup 
    SET_BIT((*timer)->CR1, 3);  /* set one pulse mode on */
    SET_BIT((*timer)->CR1, 2);  /* set URS to only counter overflow generates interrupt*/
    SET_BITS((*timer)->CR2, 4, 0x02, 0x0F);  /* set MMS to use overflow as update event */

    // Load the prescaler and generate event
    SET_BITS((*timer)->PSC, 0, BASIC_TIM_PSC, 0xFFFF);
    SET_BIT((*timer)->EGR, 0);

    return 0;
}

/**
 * @brief Run timer to delay in ms
 */
void delay_ms(
    uint16_t ms,      //! [in] Number of ms to delay by
    BASIC_TIM* timer  //! [in] Timer to use
) {
    assert(timer != NULL);
    int remaining_ms = ms;
    int current_interval_ms;

    // Run the timer
    while (remaining_ms > 0) {
        
        // if more than max, then pass in only max
        if (remaining_ms > MAX_INTERVAL_MS) {
            current_interval_ms = MAX_INTERVAL_MS;
            remaining_ms -= MAX_INTERVAL_MS;
        } else {
            current_interval_ms = remaining_ms;
            remaining_ms = 0;
        }

        // Clear the SR from previous run
        RESET_BIT(timer->SR, 0);

        // Run the timer for current_interval_ms (set ARR and start, then poll SR)
        SET_BITS(timer->ARR, 0, (current_interval_ms * 10) - 1, 0xFFFF);
        SET_BIT(timer->EGR, 0);
        SET_BIT(timer->CR1, 0);

        while (READ_BIT(timer->SR, 0) != 1);
    }
}
