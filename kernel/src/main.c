#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stm32f7.h"
#include "core/mem.h"
#include "core/scheduler.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"
#include "drivers/iwdg.h"
#include "drivers/systick.h"
#include "drivers/timer.h"
#include "drivers/uart.h"
#include "helpers/logo.h"

/* kernel globals */
static heap_manager userspace_heap_mgr;
static char sys_timestamp[64];

/* tasks */

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

    /* 
     * jump to root task (userspace stack) and we should never come back to _main
     * since nothing is allocated in main there is basically nothing left on the
     * kernel stack for this function
     */
    if (create_task(&kernel_tasks, root, NULL)) {
        goto err_state;
    }
    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] Root task initialized", sys_timestamp);

    /* testing task */
    if (create_task(&kernel_tasks, init_task, NULL)) {
        goto err_state;
    }

    systick_setup();
    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] SysTick timer initialized, context switch available", sys_timestamp);

    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] Jumping to root task...", sys_timestamp);
    sched_tick();

    /* 
     * if entered error state this should cause a kernel panic and trigger a restart of the system
     * coming soon!
     */
err_state:
    /* wait for the watchdog to reboot for now */
    read_system_clock(sys_timestamp, sizeof(sys_timestamp));
    uart_out("[%s] Kernel Panic", sys_timestamp);

    while(1);
}
