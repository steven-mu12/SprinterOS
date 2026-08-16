#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "core/mem.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    _minit(&userspace_heap_mgr);

    while (1);
}
