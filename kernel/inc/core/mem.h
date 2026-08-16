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
#define USERSPACE_STACKS_SIZE_B           (STACK_SIZE * MAX_TASKS)
#define USERSPACE_HEAP_SIZE               (256 * 1024)
#define USERSPACE_HEAP_START_ADDR         USERSPACE_START_ADDR
#define USERSPACE_HEAP_END_ADDR           (USERSPACE_HEAP_START_ADDR + USERSPACE_HEAP_SIZE)

_Static_assert((USERSPACE_HEAP_SIZE & (USERSPACE_HEAP_SIZE - 1)) == 0,
               "buddy pool must be a power of two");
_Static_assert((USERSPACE_HEAP_SIZE + USERSPACE_STACKS_SIZE_B) <= USERSPACE_SIZE_B,
               "heap plus task stacks do not fit in userspace");

/* 
 * memory buddy allocator algorithm
 */

/*
 * mapping between node index vs size vs layer for stm32f767zi

 layer | node indices | count | block size
-------+--------------+-------+-----------
   0   |            0 |     1 |    256 KB
   1   |          1-2 |     2 |    128 KB
   2   |          3-6 |     4 |     64 KB
   3   |         7-14 |     8 |     32 KB
   4   |        15-30 |    16 |     16 KB
   5   |        31-62 |    32 |      8 KB
   6   |       63-126 |    64 |      4 KB
   7   |      127-254 |   128 |      2 KB
   8   |      255-510 |   256 |      1 KB
   9   |     511-1022 |   512 |    512 B
  10   |    1023-2046 |  1024 |    256 B
-------+--------------+-------+-----------
*/
#define MEM_BUDDY_MIN_BLOCK_SIZE_B  256
#define MEM_BUDDY_MAX_BLOCKS        (2 * (USERSPACE_HEAP_SIZE / MEM_BUDDY_MIN_BLOCK_SIZE_B) - 1)
#define MEM_BUDDY_MAX_LAYER_ID      __builtin_ctz((USERSPACE_HEAP_SIZE / MEM_BUDDY_MIN_BLOCK_SIZE_B))

typedef enum node_state_t {
    NODE_FREE = 0,
    NODE_USED = 1,
    NODE_SPLIT = 2,
    NODE_INVALID = 3,
} node_state_t;

typedef struct mem_node_t {
    /* free, used, split into children somewhere in the tree, or doesnt exist */
    node_state_t state;
    tid_t owner_tid;
} mem_node_t;

typedef struct heap_manager {
    mem_node_t mem_nodes[MEM_BUDDY_MAX_BLOCKS];
} heap_manager;

/*
 * this mem allocator is init duing kernel bootup
 * the design is these are kernel functions, when exposed to the user, they do NOT have access to heap_mgr
 */
void _minit(heap_manager* heap_mgr);
address_t _malloc(heap_manager* heap_mgr, memsize_t req_size, tid_t requestor);
int _free(heap_manager* heap_mgr, address_t target);

#endif
