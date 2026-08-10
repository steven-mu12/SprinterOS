#ifndef __SPRINTER_COMMON_H__
#define __SPRINTER_COMMON_H__

#include <stdint.h>

/* general constants */
#define TID_NULL        (-1)         /* nulltask tid */
#define MAX_TASKS       16           /* max tasks system will support */
#define STACK_SIZE      0x1000       /* 4KB static blocks for now */

/* error codes */
#define RTX_OK   0
#define RTX_ERR  1
#define RTX_NOP  2

/* data types */
typedef uint32_t tid_t;
typedef uintptr_t address_t;
typedef uint16_t memsize_t;

/** 
 * @brief UNIVERSAL CONSTANTS
 */
#define SIGNATURE        0x04277DC9  // 69696969

#endif /* __SPRINTER_COMMON_H__ */
