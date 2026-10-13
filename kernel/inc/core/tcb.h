#ifndef __TCB_H__
#define __TCB_H__

#include <stdint.h>

#include "cortex_m7.h"
#include "sprinter_common.h"

typedef struct tcb_t {
    enum Status {
        STATUS_NULL = 0,
        STATUS_READY = 1,
        STATUS_RUNNING = 2,
        STATUS_SUSPENDED = 3
    } status;

    void (*ptask)(void* args);     /* callback */
    void* args;
    address_t stack_high;          /* starting address of stack */
    tid_t tid;                     /* task id */
    memsize_t stack_size;          /* stack size */
    address_t task_sp;             /* current task's sp */
    uint32_t priority;             /* higher gets picked more often */
    uint64_t vruntime;             /* weighted cpu time, the lowest one runs next */
    void* wait_on;                 /* pointer to what this task is waiting on if suspended */
} tcb_t;

#define PRIORITY_MAX                    8
#define PRIORITY_DEFAULT                0
#define VRUNTIME_BASE                   1024
#define VRUNTIME_SLICE(PRIO)            (VRUNTIME_BASE >> (PRIO))
#define VRUNTIME_PRORATE(PRIO, USED)    ((USED) / (SYSTICK_REGS->LOAD / VRUNTIME_SLICE(PRIO)))

/* system functions callbacks */
void root(void *args);
void init_task(void *args);

#endif /* __TCB_H__ */
