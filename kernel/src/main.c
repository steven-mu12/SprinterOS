#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stm32f7.h"
#include "core/mem.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"
#include "drivers/iwdg.h"
#include "drivers/timer.h"
#include "drivers/uart.h"
#include "helpers/logo.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;
static char sys_timestamp[64];

/* tasks */
static taskbuff_t tasks;
static volatile tcb_t *active_task;

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    start_system_clock();

    print_logo();

    _minit(&userspace_heap_mgr);
    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] SprinterOS heap manager initialized", sys_timestamp);

    /* actual system simple timer for approx delays */
    

    /* 
     * jump to root task (userspace stack) and we should never come back to _main
     * since nothing is allocated in main there is basically nothing left on the
     * kernel stack for this function
     */
    if (create_task(&tasks, root, NULL)) {
        goto err_state;
    }

    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] Root task initialized", sys_timestamp);

    /* 
     * right now since no userspace must go here. However once we jump to root task
     * we should never be in this loop ever
     */
err_state:
    while (1) {
        iwdg_reset();
    }
}
