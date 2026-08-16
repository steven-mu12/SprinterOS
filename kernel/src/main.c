#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stm32f7.h"
#include "core/mem.h"
#include "drivers/iwdg.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    _minit(&userspace_heap_mgr);

    while (1) {
        iwdg_reset();
    }
}
