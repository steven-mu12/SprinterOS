#include <stddef.h>
#include <stdint.h>

#include "core/mem.h"
#include "core/sprinter_common.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"

/* create task and helpers */
static int add_task(taskbuff_t *tasks, tcb_t new_task) {
    if (tasks == NULL) {
        return _ERR;
    }
    if (tasks->tasks_in_buf >= MAX_TASKS) {
        return _NOP;
    }

    /*
     * find the next free spot and set its tid and insert the task
     */
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (tasks->buffer[i].status == STATUS_NULL) {
            new_task.tid = i;
            new_task.status = STATUS_READY;
            new_task.stack_high = USERSPACE_END_ADDR - (i * STACK_SIZE);
            new_task.stack_size = STACK_SIZE;
            tasks->buffer[i] = new_task;
            tasks->tasks_in_buf++;
            return _OK;
        }
    }

    /* this should never return this */
    return _ERR;
}

int create_task(taskbuff_t* tasks, void (*callback)(void*), void* args) {
    if (tasks == NULL || callback == NULL) {
        return _ERR;
    }
    if (tasks->tasks_in_buf >= MAX_TASKS) {
        return _NOP;
    }

    tcb_t task;
    task.ptask = callback;
    task.args = args;

    return(add_task(tasks, task));
}

/* remove a task from the buffer */
int remove_task(taskbuff_t *tasks, tid_t target_tid) {
    if (tasks == NULL) {
        return _ERR;
    }

    /* you can't attempt to remove the root task */
    if (target_tid == 0 || target_tid >= MAX_TASKS) {
        return _NOP;
    }

    if (tasks->buffer[target_tid].status == STATUS_NULL) {
        return _NOP;
    }
    tasks->buffer[target_tid].status = STATUS_NULL;
    tasks->tasks_in_buf--;

    return _OK;
}

int run_task(taskbuff_t *tasks, tid_t target_tid, volatile tcb_t** current_task) {
    if (tasks == NULL || current_task == NULL) {
        return _ERR;
    }

    if (target_tid >= MAX_TASKS) {
        return _NOP;
    }

    volatile tcb_t* target_task = &(tasks->buffer[target_tid]);
    if (target_task->status == STATUS_NULL) {
        return _NOP;
    }

    /* set task to running */
    target_task->status = STATUS_RUNNING;
    *current_task = target_task;
    return _OK;
}

int suspend_task(taskbuff_t *tasks, tid_t target_tid, volatile tcb_t** current_task) {
    if (tasks == NULL || current_task == NULL) {
        return _ERR;
    }

    if (target_tid >= MAX_TASKS) {
        return _NOP;
    }

    volatile tcb_t* target_task = &(tasks->buffer[target_tid]);
    if (target_task->status == STATUS_NULL) {
        return _NOP;
    }

    /* set task to suspended */
    target_task->status = STATUS_SUSPENDED;
    *current_task = NULL;
    return _OK;
}
