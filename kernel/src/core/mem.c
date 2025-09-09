/**
 ******************************************************************************
 * @file           : mem.c
 * @author         : Steven Mu
 * @summary		   : Memory interface source for SprinterOS
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
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sprinter_common.h"
#include "mem.h"

/** 
 * @brief Initializes memory.
 */
int mem_init(
    Memory* memory  //!< [in] pointer to memory linked list
) {
    assert(memory != NULL);

    // Memory is just a big contiguous block right now, only with MemBlock stuff
    // |------------------------------ |
    // |<metadata><prev/next>--------- |
    // |------------------------------ |

    // This will not go out of scope since we're forcing it to use address of heap_start
    MemBlock* mem_head = (MemBlock*)heap_start();  // Use heap_start()'s memory address
    if ((heap_span_b() - sizeof(MemBlock)) <= 0) {
        return RTX_ERR;
    }

    // Set free mem linked list
    mem_head->prev = NULL;
    mem_head->next = NULL;
    mem_head->metadata.size= heap_span_b() - sizeof(MemBlock);
    mem_head->metadata.owner = 0;
    mem_head->metadata.signature = 0;
    mem_head->metadata.allocated = false;

    memory->head = mem_head;
    memory->tail = mem_head;

    return RTX_OK;
}

/** 
 * @brief Allocate a block of memory. Uses first-fit algorithm
 * @return the memory address of the block
 */
void* mem_alloc(
    size_t size,       //!< [in] size of block to allocate in bytes
    task_t requestor,  //!< [in] requestor of memory
    Memory* memory     //!< [in] memory linked list
) {
    assert(memory != NULL);

    // Find the first fit block to use to allocate
    uint8_t* starting_addr;  // this stores the starting addr (byte addressable) BEFORE MemBlock
    MemBlock* curr_block = memory->head;
    size_t original_size;

    if (curr_block == NULL) {
        return NULL;
    }

    bool found = false;
    bool need_to_partition = false;

    while (curr_block != NULL) {
        // if leftover space doesn't, or perfectly, fits another MemBlock, 
        // just allocate the whole thing
        if ((curr_block->metadata.signature != 0) && 
            (curr_block->metadata.signature != SIGNATURE)) {
            return NULL;
        }

        if ((curr_block->metadata.size >= size) && (!curr_block->metadata.allocated)) {
            found = true;
            starting_addr = (uint8_t*)curr_block;

            if (curr_block->metadata.size - size > sizeof(MemBlock)) {
                need_to_partition = true;
                original_size = curr_block->metadata.size;
            } else {
                need_to_partition = false;
            }
            break;
        }
        curr_block = curr_block->next;
    }

    // if we got here and found is still false, then we didn't get anything
    if (!found) {
        return NULL;
    }

    // Allocate the actual memory - if no split, just change ownership, alloc status and return;
    MemBlock* alloc_block = (MemBlock*)starting_addr;

    alloc_block->metadata.owner = requestor;
    alloc_block->metadata.allocated = true; 
    alloc_block->metadata.signature = SIGNATURE;

    if (!need_to_partition) {
        return (void*)alloc_block;
    }

    // If we need to partition, set size to be requested, and set up empty block after
    alloc_block->metadata.size = size;

    uint8_t* new_partition_addr = starting_addr + sizeof(MemBlock) + size;
    MemBlock* new_partition = (MemBlock*)new_partition_addr;

    new_partition->metadata.owner = 0;
    new_partition->metadata.signature = 0;
    new_partition->metadata.size = original_size - size - sizeof(MemBlock);
    new_partition->metadata.allocated = false;

    // Did this partition happen at the end of memory?
    new_partition->prev = alloc_block;
    if (alloc_block->next == NULL) {
        new_partition->next = NULL;
        memory->tail = new_partition;
    } else {
        new_partition->next = alloc_block->next;
        new_partition->next->prev = new_partition;
    }
    alloc_block->next = new_partition;

    return (void*)alloc_block;
}

/** 
 * @brief Deallocate a block of memory
 * @return Error code
 * @note This does NOT run coalescence. Will be done in seperate housekeeping thread
 */
int mem_free(
    void* block,    //!< [in] block to free
    Memory* memory  //!< [in] memory linked list
) {
    assert(block != NULL);
    assert(memory != NULL);

    // Check that the memory that the user provides is valid
    MemBlock* user_mem = (MemBlock*)block;

    if (user_mem->metadata.signature == 0) {
        return RTX_NOP;  // already freed
    } 
    if (user_mem->metadata.signature != SIGNATURE) {
        return RTX_ERR;  // user passed in bad address
    }

    // Deallocate (we checked that signature is SIGNATURE)
    user_mem->metadata.allocated = false;
    user_mem->metadata.owner = 0;
    user_mem->metadata.signature = 0;

    return RTX_OK;
}
