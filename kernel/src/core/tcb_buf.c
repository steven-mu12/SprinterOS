#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "core/sprinter_common.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"

int add_task(taskbuff_t *tasks, tcb_t *new_task) {
    assert(tasks != NULL);
    assert(new_task != NULL);

    if (tasks->tasks_in_buf >= MAX_TASKS) {
        return 1;
    }

    /* insert into buffer */
    tasks->buffer[tasks->tasks_in_buf] = *new_task;
    tasks->tasks_in_buf++;

    return 0;
}

int remove_task(taskbuff_t *tasks, tid_t target_tid) {
    assert(tasks != NULL);
    if (target_tid < 0) {
        return 1;
    }

    /* just find and remove the task from the queue */
    int indice_of_target = -1;
    for (int i=0; i < tasks->tasks_in_buf; i++) {
        if (tasks->buffer[i].tid == target_tid) {
            indice_of_target = i;
            break;
        }
    }
    if (indice_of_target == -1) {
        /* task not found */
        return 1;
    }

    /* move everything behind it up */
    for (int i=indice_of_target; i < tasks->tasks_in_buf - 1; i++) {
        tasks->buffer[i] = tasks->buffer[i + 1];
    }
    tasks->tasks_in_buf--;

    return 0;
}

int run_task(taskbuff_t *tasks, tid_t target_tid) {
    assert(tasks != NULL);
    if (target_tid < 0) {
        return 1;
    }

    /* find the task and set it as running */
    tcb_t* target_task = NULL;
    for (int i=0; i < tasks->tasks_in_buf; i++) {
        if (tasks->buffer[i].tid == target_tid) {
            target_task = &(tasks->buffer[i]);
        }
    }
    if (target_task == NULL) {
        return 1;
    }

    /* set task to running */
    target_task->status = STATUS_RUNNING;
    return 0;
}

int suspend_task(taskbuff_t *tasks, tid_t target_tid) {
    assert(tasks != NULL);
    if (target_tid < 0) {
        return 1;
    }

    /* find the task, and suspend if running */
    tcb_t* target_task = NULL;
    for (int i=0; i < tasks->tasks_in_buf; i++) {
        if (tasks->buffer[i].tid == target_tid) {
            target_task = &(tasks->buffer[i]);
        }
    }
    if (target_task == NULL) {
        return 1;
    }

    /* set task to suspended */
    target_task->status = STATUS_SUSPENDED;
    return 0;;
}
