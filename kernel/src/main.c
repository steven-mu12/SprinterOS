#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stm32f7.h"
#include "core/mem.h"
#include "drivers/iwdg.h"
#include "drivers/uart.h"
#include "helpers/logo.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    print_logo();

    _minit(&userspace_heap_mgr);

    /* no timer yet so stamp is just temp */
    uart_out("[0.000000] SprinterOS heap manager initialized");

    while (1) {
        iwdg_reset();
    }
}
