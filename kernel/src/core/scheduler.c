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
    tcb_t* best = NULL;

    if (tasks == NULL) {
        return NULL;
    }

    /* whoever has had the least weighted cpu time so far wins */
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        volatile tcb_t* task = &tasks->buffer[i];
        if (task->status != STATUS_READY) {
            continue;
        }
        if (best == NULL || task->vruntime < best->vruntime) {
            best = (tcb_t*)task;
        }
    }

    /*
     * the running task has to be strictly lower to be scheduled
     */
    if (current_task != NULL && current_task->status == STATUS_RUNNING) {
        if (best == NULL || current_task->vruntime < best->vruntime) {
            return current_task;
        }
    }

    return best;
}

static char sys_timestamp[64];

void sched_tick(void) {
    tcb_t* next;

    /* add runtime that was just given to the current task */
    if (current_task != NULL) {
        current_task->vruntime += VRUNTIME_SLICE(current_task->priority);
    }

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

/* yields the current running task instead of waiting for next tick */
void sched_yield(void) {
    if (current_task == NULL) {
        /* a yield should never happen when there is no current task, if it does something is horribly wrong */
        /* should actually kernel panic here - add once mechanics added */
        return;
    }

    /*
     * all of this has to be atomic against systick. a switch applied half way
     * leaves a task marked running that nothing will ever pick again, and the
     * symptom turns up minutes later nowhere near here
     */
    irq_disable();

    current_task->vruntime += VRUNTIME_PRORATE(current_task->priority, SYSTICK_REGS->LOAD - SYSTICK_REGS->VAL);
    tcb_t* next = _scheduler(&kernel_tasks, (tcb_t*)current_task);

    if (next != NULL && next != current_task) {
        /* hand the slice over */
        current_task->status = STATUS_READY;
        next->status = STATUS_RUNNING;
        next_task = next;

        SYSTICK_REGS->VAL = 0; /* this is to reload so the next task gets the full slice */
        SCB_REGS->ICSR = SET_BITMASK(28);
    }

    irq_enable();
}
