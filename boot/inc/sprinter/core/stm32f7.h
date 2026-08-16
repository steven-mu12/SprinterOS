#ifndef __STM32F7_H__
#define __STM32F7_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stm32f7.h>

#include "sprinter/core/cmsis_device.h"

#include "external/cmsis/core_cm7.h"

/** 
 * globally known variables - anything that includes this header will have access
 */
typedef struct BASIC_TIM BASIC_TIM;
extern BASIC_TIM* __global_simple_timer_ptr__;

#endif
