#ifndef __SPRINTER_COMMON_H__
#define __SPRINTER_COMMON_H__

#include <stdint.h>

/* general constants */
#define TID_NULL        (-1)         /* nulltask tid */
#define MAX_TASKS       16           /* max tasks system will support */
#define STACK_SIZE      0x1000       /* 4KB static blocks for now */

/* error codes */
#define _OK   0
#define _ERR  -1
#define _NOP  1

/* data types */
typedef uint32_t tid_t;
typedef uintptr_t address_t;
typedef uint32_t memsize_t;

/*
 * UNIVERSAL CONSTANTS
 */
#define SIGNATURE        0x04277DC9  // 69696969

/*
 * USEFUL HELPERS
 */
static inline uint32_t round_up_to_power_of_2(uint32_t val) {
    if (val <= 1) { return 1; }
    return (1 << (sizeof(uint32_t) * 8 - __builtin_clz(val - 1)));
}

#endif /* __SPRINTER_COMMON_H__ */
