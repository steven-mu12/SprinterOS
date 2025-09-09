/**
 ******************************************************************************
 * @file           : mem.h
 * @author         : Steven Mu
 * @summary		   : Memory interface header for SprinterOS
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
#ifndef __MEM_H__
#define __MEM_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sprinter_common.h"

/** 
 * @brief Metadata field of memory block
 */
typedef struct {
    bool allocated;
    uint32_t signature;
    size_t size;
    task_t owner;
} Metadata; 

/** 
 * @brief Allocated memory block (node in linked list)
 * @note since we're using itself, forward declare with tag
 */
typedef struct MemBlock {
    Metadata metadata;  //!< Metadata of current block
    struct MemBlock* next;      //!< Next memory block in linked list
    struct MemBlock* prev;      //!< Previous memory block in linked list
} MemBlock;

/** 
 * @brief Global memory infrastructure
 */
typedef struct {
    MemBlock* head;  //!< First block in memory
    MemBlock* tail;  //!< Last allocated block in memory
} Memory;

/** 
 * @brief Heap management variables
 */
extern char _end;              // start of heap
extern char _estack;           // top of RAM
extern size_t _Min_Stack_Size;      // from the script

// Needs to be static members, so unique instance for each linkage
// Inline to reduce function call overhead
static inline uint8_t* heap_start(void) { return &_end; }
static inline uint8_t* heap_end(void) { return (uint8_t*)(&_estack - _Min_Stack_Size); }
static inline size_t heap_span_b(void) { return (size_t)(heap_end() - heap_start()); }

/** 
 * @brief User functions
 */
int mem_init(Memory* memory);
void* mem_alloc(size_t size, task_t requestor, Memory* memory);
int mem_free(void* block, Memory* memory);

#endif