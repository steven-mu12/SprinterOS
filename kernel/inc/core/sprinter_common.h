/**
 ******************************************************************************
 * @file           : sprinter_common.h
 * @author         : Steven Mu
 * @summary		   : Sprinter OS Common Definitions
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

#ifndef __SPRINTER_COMMON_H__
#define __SPRINTER_COMMON_H__

#include <stdint.h>

/** 
 * @brief GENERAL TCB CONSTANTS
 */
#define TID_NULL        (-1)     //!< predefined Task ID for the NULL task
#define MAX_TASKS       16       //!< maximum number of tasks in the system
#define MIN_STACK_SIZE  0x200    //!< min. size of each task’s stack

/** 
 * @brief ERROR CODE
 */
#define RTX_OK   0
#define RTX_ERR  1
#define RTX_NOP  2

/** 
 * @brief DATA TYPES
 */
typedef uint32_t task_t;     //!< Task ID (TID)
typedef uintptr_t Address;   //!< Pointer addresses
typedef uint16_t MemSize;    //!< 16 bit memory size

/** 
 * @brief UNIVERSAL CONSTANTS
 */
#define SIGNATURE        0x04277DC9  // 69696969

#endif /* __SPRINTER_COMMON_H__ */
