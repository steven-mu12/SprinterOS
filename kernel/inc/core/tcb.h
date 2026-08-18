#ifndef __TCB_H__
#define __TCB_H__

#include <stdint.h>

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
    address_t stack_high;          /* stating address of stack */
    tid_t tid;                     /* task id */
    memsize_t stack_size;          /* stack size */
} tcb_t;

/* root function callback */
void root(void *args);

#endif /* __TCB_H__ */
