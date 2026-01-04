/**
 ******************************************************************************
 * @file           : tcb.h
 * @author         : Steven Mu
 * @summary		   : Task Control Block Structure
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

#ifndef __TCB_H__
#define __TCB_H__

#include <stdint.h>

#include "core/sprinter_common.h"

/** 
 * @brief Sprinter Task Control Block
 */
typedef struct task_control_block{
    /* Status field of possible task status */
    enum Status {
        STATUS_NULL = 0,
        STATUS_READY = 1,
        STATUS_RUNNING = 2,
        STATUS_SUSPENDED = 3
    } status;

    void (*ptask)(void* args);   //!< callback of function to run in task
    Address stack_high;          //!< starting address of stack
    task_t tid;                  //!< task ID
    MemSize stack_size;          //!< stack size. Must be a multiple of 8


#endif /* __TCB_H__ */
