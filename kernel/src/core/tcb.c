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
#define LOGIN_PROMPT    "login > "

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

/*
 * SHELL TASK AND SHELL TASK HANDLERS
 */
static void shell_run(char* line) {
    (void)line;
}

/* 
 * nothing right now, just checks a list of users (pretty scuffed rn ik)
 * after we add the fs we'll do like a protected passwd system like linux
 */
static char* users[] = {
    "root",
    "smu",
};
static int valid_user(char* name) {
    if (name == NULL) {
        return 0;
    }
    for (uint32_t i = 0; i < 2; i++) {
        if (strcmp(name, users[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void shell(void *args) {
    char line[SHELL_LINE_MAX];
    uint32_t len = 0;

    (void)args;

    /* login prompt */
    int logged_in = 0;
    uart_out("");
    uart_out("");
    uart_output_str("Welcome to SprinterOS\r\n");
    uart_output_str(LOGIN_PROMPT);

    while (1) {
        char character = uart_read();

        /* newline characters */
        if (character == '\r' || character == '\n') {
            line[len] = '\0';
            uart_out("");

            /* enforce the login */
            if (!logged_in) {
                if (valid_user(line)) {
                    uart_output_str(SHELL_PROMPT);
                    logged_in = 1;
                    len = 0;
                    continue;
                }
                len = 0;
                uart_output_str("Incorrect user, please try again\r\n");
                uart_output_str(LOGIN_PROMPT);
                continue;
            }

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
