#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>

/**
 * internal watchdog structure
 */
struct iwdg {
	volatile uint32_t KR, PR, RLR, SR, WINR;
};
#define IWDG ((struct iwdg *) IWDG_BASE)

/**
 * user functions
 */
int iwdg_reset(void);
//int iwdg_bypass(void);

#endif
