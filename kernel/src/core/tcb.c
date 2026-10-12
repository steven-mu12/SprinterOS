#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "core/scheduler.h"
#include "core/tcb.h"
#include "core/tcb_buf.h"
#include "drivers/iwdg.h"
#include "drivers/timer.h"
#include "drivers/uart.h"

#define SHELL_LINE_MAX  128
#define SHELL_PROMPT    "sprinter > "

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

static void shell_run(char* line) {
    (void)line;
}

void shell(void *args) {
    char line[SHELL_LINE_MAX];
    uint32_t len = 0;

    (void)args;

    uart_output_str(SHELL_PROMPT);

    while (1) {
        char character = uart_read();

        /* newline characters */
        if (character == '\r' || character == '\n') {
            line[len] = '\0';
            uart_out("");
            shell_run(line);
            len = 0;
            uart_output_str(SHELL_PROMPT);

        /* backspace and delete */
        } else if (character == 0x08 || character == 0x7F) {
            if (len > 0) {
                len--;
                uart_output_str("\b \b");
            }

        } else if (len < SHELL_LINE_MAX - 1) {
            line[len++] = character;
            uart_output_char(character);
        }
    }
}
