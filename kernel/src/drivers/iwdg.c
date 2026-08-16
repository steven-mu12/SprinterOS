#include <stdint.h>

#include "drivers/iwdg.h"

#include "stm32f7.h"

int iwdg_reset(void) {
	IWDG->KR = 0xAAAA;
	return 0;
}
