#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/*
 * SPRINTEROS KERNEL MAIN FUNCTION
 */
int _main(void) {
    while (1) {

    }
}

/*
 * SPRINTEROS KERNEL ENTRY POINT
 * need this wrapper because when we add multithreading, need a central function
 * that spins up the threads & manages them + the global resources
 */
int main(void) {
    // Run the main kernel functionality (no threading for now)
    _main();
}
