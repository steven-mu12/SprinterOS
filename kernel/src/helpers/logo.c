#include "helpers/logo.h"

#include "core/mem.h"
#include "drivers/uart.h"

#define SPRINTER_VERSION "0.1.0"

void print_logo(void) {
    uart_out("");
    uart_out("   ____             _       _            ___  ____");
    uart_out("  / ___| _ __  _ __(_)_ __ | |_ ___ _ __/ _ \\/ ___|");
    uart_out("  \\___ \\| '_ \\| '__| | '_ \\| __/ _ \\ '__| | | \\___ \\");
    uart_out("   ___) | |_) | |  | | | | | ||  __/ |  | |_| |___) |");
    uart_out("  |____/| .__/|_|  |_|_| |_|\\__\\___|_|   \\___/|____/");
    uart_out("        |_|");
    uart_out("");

    uart_out("[0.000000] SprinterOS version %s (gcc %s) %s %s",
             SPRINTER_VERSION, __VERSION__, __DATE__, __TIME__);
    uart_out("[0.000000] CPU: ARM Cortex-M7, STM32F767ZI");
    uart_out("[0.000000]");
    uart_out("[0.000000] Memory map:");
    uart_out("[0.000000]   kernel bss    %h - %h  %d KB",
             KERNELSPACE_START_ADDR, KERNELSPACE_HEAP_START_ADDR,
             (KERNELSPACE_HEAP_START_ADDR - KERNELSPACE_START_ADDR) / 1024);
    uart_out("[0.000000]   kernel heap   %h - %h  %d KB",
             KERNELSPACE_HEAP_START_ADDR, KERNELSPACE_HEAP_END_ADDR,
             (KERNELSPACE_HEAP_END_ADDR - KERNELSPACE_HEAP_START_ADDR) / 1024);
    uart_out("[0.000000]   kernel stack  %h - %h  %d KB",
             KERNELSPACE_HEAP_END_ADDR, KERNELSPACE_END_ADDR,
             (KERNELSPACE_END_ADDR - KERNELSPACE_HEAP_END_ADDR) / 1024);
    uart_out("[0.000000]   user heap     %h - %h  %d KB",
             USERSPACE_HEAP_START_ADDR, USERSPACE_HEAP_END_ADDR,
             USERSPACE_HEAP_SIZE / 1024);
    uart_out("[0.000000]   task stacks   %h - %h  %d KB",
             USERSPACE_HEAP_END_ADDR, USERSPACE_HEAP_END_ADDR + USERSPACE_STACKS_SIZE_B,
             USERSPACE_STACKS_SIZE_B / 1024);
    uart_out("[0.000000]   kernel image  %h - %h  %d KB",
             KERNEL_IMG_ORIGIN, KERNEL_IMG_ORIGIN + KERNEL_IMG_SIZE_B,
             KERNEL_IMG_SIZE_B / 1024);
    uart_out("[0.000000]");
    uart_out("[0.000000] Buddy allocator: %d KB pool, %d B min block, %d nodes",
             USERSPACE_HEAP_SIZE / 1024, MEM_BUDDY_MIN_BLOCK_SIZE_B, MEM_BUDDY_MAX_BLOCKS);
    uart_out("[0.000000] Tasks: %d max, %d KB stack each", MAX_TASKS, STACK_SIZE / 1024);
    uart_out("[0.000000]");
}
