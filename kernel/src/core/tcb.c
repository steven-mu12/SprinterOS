#include <stddef.h>
#include <stdint.h>

#include "core/tcb.h"

/* 
 * root function basically a nulltask right now
 */
void root(void *args) {
    (void)args;
    while(1);
}
