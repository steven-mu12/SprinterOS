#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cortex_m7.h"
#include "drivers/uart.h"
#include "shell/shell.h"
#include "stm32f7.h"

static void cmd_reboot(char* args);

static struct shell_cmd commands[] = {
    { "reboot", "restart the system", cmd_reboot },
};

static void cmd_reboot(char* args) {
    (void)args;

    uart_out("rebooting...");

    SCB_REGS->AIRCR = (0x5FA << 16) | SET_BITMASK(2);
    __asm volatile ("dsb" ::: "memory");
    while (1);
}

void shell_run(char* line) {
    char* args = line;

    if (line == NULL || line[0] == '\0') {
        return;
    }

    while (*args != '\0' && *args != ' ') {
        args++;
    }
    if (*args == ' ') {
        *args = '\0';
        args++;
    }

    for (uint32_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strcmp(line, commands[i].name) == 0) {
            commands[i].run(args);
            return;
        }
    }

    uart_out("unknown command: %s", line);
}
