/**
 ******************************************************************************
 * @file           : tcb_buf.h
 * @author         : Steven Mu
 * @summary		   : Task Control Block Buffer, used to store tasks
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
 ******************************************************************************
 */

#ifndef __TCB_BUF_H__
#define __TCB_BUF_H__

#include <stdint.h>

#include "core/sprinter_common.h"
#include "core/tcb.h"

/** 
 * @brief TaskBuf structure, used to store multiple TCBs
 */
typedef struct {
	volatile TCB buffer[MAX_TASKS];    //!< static buffer to store tasks
	volatile task_t tids[MAX_TASKS];  //!< queue of TID's for faster access
	volatile uint32_t tasks_in_queue;  //!< number of tasks in a queue
} TaskBuf; 

/**
 * @brief Task buffer user functionality
 */
int add_task(TaskBuf *tasks, TCB *new_task);
int remove_task(TaskBuf *tasks, task_t target);
int run_task(TaskBuf *tasks, task_t target);
int suspend_task(TaskBuf *tasks, task_t target);

#endif /* __TCB_BUF_H__ */
