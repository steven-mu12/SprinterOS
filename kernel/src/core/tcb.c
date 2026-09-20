#include <stddef.h>
#include <stdint.h>

#include "core/tcb.h"
#include "drivers/iwdg.h"
#include "drivers/uart.h"

/* 
 * root function basically a nulltask right now
 */
void root(void *args) {
    (void)args;
    while(1) {
        iwdg_reset();
    }
}

void init_task(void *args) {
    (void)args;
    while(1) {
        iwdg_reset();
    }
}
