#include <stddef.h>
#include <stdint.h>

#include "cortex_m7.h"
#include "stm32f7.h"

#include "core/mem.h"
#include "core/scheduler.h"
#include "core/sprinter_common.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"

#include "drivers/timer.h"
#include "drivers/uart.h"

/* switch.s swaps between these two, it never picks them */
volatile tcb_t* current_task;
volatile tcb_t* next_task;

taskbuff_t kernel_tasks;

/* switch.s reaches into the tcb by a hardcoded offset, keep the two in step */
_Static_assert(offsetof(tcb_t, task_sp) == TCB_SP_OFFSET, "TCB_SP_OFFSET in switch.s is stale");

tcb_t* _scheduler(taskbuff_t* tasks, tcb_t* current_task) {
    /* 
     * go through and find the next task in the TCB
     * better algorithm incoming
     */
    int index = -1;
    for (int i=0; i < MAX_TASKS; i++) {
        if (&tasks->buffer[i] == current_task) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return NULL;
    }

    for (int i = 1; i < MAX_TASKS; i++) {
        int next = (index + i) % MAX_TASKS;

        if (tasks->buffer[next].status == STATUS_READY) {
            return (tcb_t*)&tasks->buffer[next];
        }
    }

    /* if the above didnt return anything that means nothing else so run again */
    return current_task;
}

static char sys_timestamp[64];

void sched_tick(void) {
    tcb_t* next;

    if (current_task == NULL) {
        if (kernel_tasks.buffer[0].status != STATUS_READY) {
            return;
        }
        next = (tcb_t*)&kernel_tasks.buffer[0];
    } else {
        next = _scheduler(&kernel_tasks, (tcb_t*)current_task);
    }

    if (next == NULL || next == current_task) {
        return;
    }

    /* hand the slice over, the outgoing one goes back in the running for later */
    if (current_task != NULL) {
        current_task->status = STATUS_READY;
    }
    next->status = STATUS_RUNNING;
    next_task = next;

    /* testing prints */
    // read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    // uart_out("[%s] Context switch scheduled for tid=%d", sys_timestamp, next_task->tid);

    /* set pendsv to indicate ready to context switch */
    SCB_REGS->ICSR = SET_BITMASK(28);
}
