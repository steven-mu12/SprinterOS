/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Steven Mu
 * @summary		   : Main kernel entrypoint
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

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "core/sprinter_common.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"

/** 
 * @brief SPRINTEROS KERNEL MAIN FUNCTION
 * @note functionality for the 
 */
int _main(TaskBuf* tasks) {

    // main loop
    while (1) {

    }
}

/** 
 * @brief SPRINTEROS KERNEL ENTRY POINT
 * @note need this wrapper because when we add multithreading, need a central function
 *       that spins up the threads & manages them + the global resources
 */
int main(void) {
    // Set up global task buffer
    TaskBuf* tasks;

    // Run the main kernel functionality (no threading for now)
    _main(tasks);
}