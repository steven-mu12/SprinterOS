/**
 ******************************************************************************
 * @file           : tcb_buf.c
 * @author         : Steven Mu
 * @summary		   : TCB Buffer Source
 ******************************************************************************
 * MIT License

 * Copyright (c) 2025 Steven Mu

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 **********
 */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "core/sprinter_common.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"

/**
 * @brief User Functions
 */

/**
 * @brief Add a task to task buffer.
 * @return 0 if no error, 1 if error.
 */
int add_task(
    TaskBuf *tasks,  //!< [in] task buffer
    TCB *new_task    //!< [in] new task to insert
) {
    assert(tasks != NULL);
    assert(new_task != NULL);
    if (tasks->tasks_in_queue >= MAX_TASKS) {
        return 1;
    }

    // Iterate to insert into buffer and to back of queue
    int counter = 0;
    TCB curr_tcb;
    while (counter < MAX_TASKS) {
        curr_tcb = tasks->buffer[counter];
        if (curr_tcb.status == STATUS_NULL) {
            tasks->buffer[counter] = *new_task;
            tasks->tids[counter] = new_task->tid;
            tasks->tasks_in_queue++;
            break;
        }
        counter++;
    }
    return 0;
}

/**
 * @brief Remove a task from task buffer
 * @return 0 if something removed, 1 if nothing removed
 */
int remove_task(
    TaskBuf *tasks,  //!< [in] task buffer
    task_t target    //!< [in] target TID to remove
) {
    assert(tasks != NULL);
    if (target == 0) {
        return 1;
    }

    // find and remove the task from queue
    // - another thread will determine and run scheduler
    for (int i=0; i < tasks->tasks_in_queue; i++) {
        if (tasks->tids[i] == target) {
            tasks->buffer[i].status = STATUS_NULL;
            tasks->tids[i] = 0;
            tasks->tasks_in_queue--;
            return 0;
        }
    }
    // if we got here, then we found nothing
    return 1;
}

/**
 * @brief Put a task into running mode
 * @return 0 if successfully run. 1 if not found or error
 */
int run_task(
    TaskBuf *tasks,  //!< [in] task buffer
    task_t target    //!< [in] target TID to run
) {
    assert(tasks != NULL);
    if (target == 0) {
        return 1;
    }

    // Make sure nothing else is running (also, grab target)
    TCB* target_task = NULL;
    for (int i=0; i < tasks->tasks_in_queue; i++) {
        if (tasks->buffer[i].status == STATUS_RUNNING) {
            return 1;
        }

        if (tasks->tids[i] == target) {
            target_task = &(tasks->buffer[i]);
        }
    }

    // See if we have a valid target tasks
    if (target_task == NULL) {
        return 1;
    }

    // Set task to running
    target_task->status = STATUS_RUNNING;
    return 0;
}

/**
 * @brief Put a task into suspend mode
 * @return 0 if successfully suspended. 1 if not found or error
 */
int suspend_task(
    TaskBuf *tasks,  //!< [in] task buffer
    task_t target    //!< [in] target TID to suspend
) {
    assert(tasks != NULL);
    if (target == 0) {
        return 1;
    }

    // Find the target, and make sure it's running
    for (int i=0; i < tasks->tasks_in_queue; i++) {
        if (tasks->tids[i] == target) {
            tasks->buffer[i].status = STATUS_SUSPENDED;
            return 0;
        }
    }
    return 1;
}
