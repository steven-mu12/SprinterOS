#ifndef __MEM_H__
#define __MEM_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "memmap_config.h"
#include "sprinter_common.h"

/* 
 * memory regions used for userspace & kernelspace
 */
extern uint8_t _dtcm_start[];
extern uint8_t _dtcm_end[];
extern uint8_t _userspace_start[];
extern uint8_t _userspace_end[];
extern uint8_t _kernel_stack_size[];
extern uint8_t _end[];

#define USERSPACE_START_ADDR        ((uint32_t)_userspace_start)
#define USERSPACE_END_ADDR          ((uint32_t)_userspace_end)

#define KERNELSPACE_START_ADDR      ((uint32_t)_dtcm_start)
#define KERNELSPACE_END_ADDR        ((uint32_t)_dtcm_end)

/* kernel heap runs from the end of .bss up to the bottom of the kernel stack */
#define KERNELSPACE_HEAP_START_ADDR   ((uint32_t)_end)
#define KERNELSPACE_HEAP_END_ADDR     ((uint32_t)_dtcm_end - (uint32_t)_kernel_stack_size)
/*  
    KERNEL MEMORY LAYOUT
    -----------------------------------------------------------------------------
    | .isr_vec_table | .txt | .data | .bss | HEAP   .....     | STACK     ..... | 
    -----------------------------------------------------------------------------
    _dctm_start                            _end                _kernel_stack_size
*/

/* 
 * userspace is sectioned by 4KB - right now, each task just gets a 4KB piece of the stack 
 * TODO virtual memory & proper paging coming soon
 */          
#define USERSPACE_HEAP_SIZE               (USERSPACE_SIZE_B - (STACK_SIZE * MAX_TASKS))
#define USERSPACE_HEAP_START_ADDR         USERSPACE_START_ADDR
#define USERSPACE_HEAP_END_ADDR           (USERSPACE_HEAP_START_ADDR + USERSPACE_HEAP_SIZE)

/* 
 * memory buddy allocator algorithm
 */
#define MEM_BUDDY_MIN_BLOCK_SIZE_B  256
#define MEM_BUDDY_MAX_BLOCKS        (2 * (USERSPACE_HEAP_SIZE / 256) - 1)

typedef enum node_state_t {
    free = 0,
    used = 1,
    intermediate = 2,
} node_state_t;

typedef struct mem_node_t {
    node_state_t state; /* this describes whether if this is a leaf node, or has valid children */
    tid_t owner;
} mem_node_t;

typedef struct mem_allocator {
    mem_node_t mem_nodes[MEM_BUDDY_MAX_BLOCKS];
} mem_allocator;

/* this mem allocator is init duing kernel bootup */

#endif
