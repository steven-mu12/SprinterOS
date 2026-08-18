#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stm32f7.h"
#include "core/mem.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"
#include "drivers/iwdg.h"
#include "drivers/uart.h"
#include "helpers/logo.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;

/* tasks */
static taskbuff_t tasks;
static volatile tcb_t *active_task;

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    print_logo();

    _minit(&userspace_heap_mgr);
    /* no timer yet so stamp is just temp */
    uart_out("[0.000000] SprinterOS heap manager initialized");

    /* 
     * jump to root task (userspace stack) and we should never come back to _main
     * since nothing is allocated in main there is basically nothing left on the
     * kernel stack for this function
     */
    if (create_task(&tasks, root, NULL)) {
        goto err_state;
    }

    /* 
     * right now since no userspace must go here. However once we jump to root task
     * we should never be in this loop ever
     */
err_state:
    while (1) {
        iwdg_reset();
    }
}
