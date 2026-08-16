#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sprinter_common.h"
#include "mem.h"

static void recursively_mark(heap_manager* heap_mgr, int i, node_state_t state) {
    if (i >= MEM_BUDDY_MAX_BLOCKS) {
        return;
    } else {
        heap_mgr->mem_nodes[i].state = state;

        recursively_mark(heap_mgr, (2 * i) + 1, state);
        recursively_mark(heap_mgr, (2 * i) + 2, state);
    }
}

void _minit(heap_manager* heap_mgr) {
    heap_mgr->mem_nodes[0].state = NODE_FREE;
    recursively_mark(heap_mgr, 1, NODE_INVALID);
    recursively_mark(heap_mgr, 2, NODE_INVALID);
}

/*
 * malloc and helper functions
 * basically the algorithm is check for free spots at layer
 * - if so, allocate and ensure all children are recursively invalid
 * - if not then recursively go up layers. If FREE, then split down however many times needed
 */
static address_t allocate(heap_manager* heap_mgr, tid_t requestor, int i, int layer) {
    /* recusively mark children as invalid */
    heap_mgr->mem_nodes[i].state = NODE_USED;
    heap_mgr->mem_nodes[i].owner_tid = requestor;
    recursively_mark(heap_mgr, (2 * i) + 1, NODE_INVALID);
    recursively_mark(heap_mgr, (2 * i) + 2, NODE_INVALID);

    /* return the address of the block allocated */
    uint32_t start_index = (1 << layer) - 1;
    uint32_t offset_in_layer = i - start_index;
    memsize_t layer_block_size = USERSPACE_HEAP_SIZE >> layer;

    return (address_t)(USERSPACE_HEAP_START_ADDR + layer_block_size * offset_in_layer);
}

static uint32_t split(heap_manager* heap_mgr, memsize_t req_size, uint32_t i, uint32_t layer, uint32_t target_layer) {
    while (layer < target_layer) {
        heap_mgr->mem_nodes[i].state = NODE_SPLIT;
        heap_mgr->mem_nodes[(2 * i) + 1].state = NODE_FREE;
        heap_mgr->mem_nodes[(2 * i) + 2].state = NODE_FREE;

        i = (2 * i) + 1;
        layer++;
    }

    return i;
}

address_t _malloc(heap_manager* heap_mgr, memsize_t req_size, tid_t requestor) {
    if ((req_size == 0) || (req_size > USERSPACE_HEAP_SIZE)) {
        return _ERR;
    }

    /* allocated block size is nearest rounded up power of 2 */
    memsize_t block_size_needed = round_up_to_power_of_2(req_size);
    if (block_size_needed <= MEM_BUDDY_MIN_BLOCK_SIZE_B) {
        block_size_needed = MEM_BUDDY_MIN_BLOCK_SIZE_B;
    }
    uint32_t layer = __builtin_clz(block_size_needed) - __builtin_clz(USERSPACE_HEAP_SIZE);
    uint32_t start_index = (1 << layer) - 1;
    uint32_t end_index = ((1 << (layer+1)) - 1);

    /* check for any free spots at layer, if so allocate and ensure all children are recursively invalid */
    for (uint32_t i = start_index; i < end_index; i++) {
        if (heap_mgr->mem_nodes[i].state == NODE_FREE) {
            return allocate(heap_mgr, requestor, i, layer);
        }
    }

    /* go up to find the first free block and split down to the right size */
    uint32_t target_layer = layer;
    while (layer > 0) {
        layer--;
        start_index = (1 << layer) - 1;
        end_index = (1 << (layer+1)) - 1;
        for (uint32_t i = start_index; i < end_index; i++) {
            if (heap_mgr->mem_nodes[i].state == NODE_FREE) {
                uint32_t final_i = split(heap_mgr, req_size, i, layer, target_layer);
                return allocate(heap_mgr, requestor, final_i, target_layer);
            }
        }
    }

    return _ERR;
}

/*
 * free and helper functions
 * free the block that is allocated and then coalesce above recursively if possible
 */
static void coalesce(heap_manager* heap_mgr, uint32_t i) {
    while (i != 0) {
        uint32_t parent = (i - 1) / 2;
        uint32_t sibling = 0;
        if ((i % 2) == 1) {
            sibling = i + 1;
        } else {
            sibling = i - 1;
        }

        /* if sibling is allocated then can't coalesce */
        if (heap_mgr->mem_nodes[sibling].state != NODE_FREE) {
            break;
        }

        heap_mgr->mem_nodes[i].state = NODE_INVALID;
        heap_mgr->mem_nodes[sibling].state = NODE_INVALID;
        heap_mgr->mem_nodes[parent].state = NODE_FREE;
        i = parent;
    }
}

int _free(heap_manager* heap_mgr, address_t target) {
    if ((target < USERSPACE_HEAP_START_ADDR) || (target >= USERSPACE_HEAP_END_ADDR)) {
        return _ERR;
    }

    /*
     * find the node to remove (must walk, since malloc(256) and malloc(1024) could technically
     * give the same address)
     */
    address_t offset = target - USERSPACE_HEAP_START_ADDR;
    memsize_t size = USERSPACE_HEAP_SIZE;
    uint32_t i = 0;

    while (heap_mgr->mem_nodes[i].state == NODE_SPLIT) {
        size /= 2;
        if (offset < size) {
            i = (2 * i) + 1;
        } else {
            offset -= size;
            i = (2 * i) + 2;
        }
    }

    if ((heap_mgr->mem_nodes[i].state != NODE_USED) || (offset != 0)) {
        return _ERR;
    }

    heap_mgr->mem_nodes[i].state = NODE_FREE;
    heap_mgr->mem_nodes[i].owner_tid = 0;
    coalesce(heap_mgr, i);

    return _OK;
}
