#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "sprinter/core/memmap.h"
#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals.h"

/** 
 * MAIN BOOT EXECUTIVE CONFIGURATIONS 
 */
/* TODO (smu): Migrate this to a config file */
#define VERSION        "0.0.3"
#define BUILD_DATE     "2026-08-02"
#define UART_COMM_PORT 1

#define TEST_SYSCLK    0
#define RECOVERY_MDOE  0

/** 
 * SPRINTEROS BOOTLOADER ERROR STATE (LED DEBUGGING)
 * 
 * blink the heartbeat in the event there is an error before UART is brought up
 * 
 * type 1 means happened at sysclk set, 2 means happened at timer setup, 3 means at UART setup
 * type 0 means anything else after we got UART (since we can now print stuff)
 */
int error(uint8_t type) {
    // blink onboard LED type amount of times for the error
    uint16_t led_pin = PIN('B', 7);
    gpio_pinmode(led_pin, GPIO_MODE_OUTPUT, 0x00);

    while (1) {
        if (type == 0) {
            while (1) {
                gpio_digital_write_sys(led_pin, 1);
                for (volatile int i=0; i< 4000000; i++);
                gpio_digital_write_sys(led_pin, 0);
                for (volatile int i=0; i< 4000000; i++);

                iwdg_reset();
            }

        } else {
            for (int j = 0; j < type; j++) {
                gpio_digital_write_sys(led_pin, 1);
                for (volatile int i=0; i< 4000000; i++);
                gpio_digital_write_sys(led_pin, 0);
                for (volatile int i=0; i< 4000000; i++);
            }
            for (volatile int i=0; i< 24000000; i++);

            /* no worries about watchdog in this case because it hasnt been set up yet */
        }
    }
}

/** 
 * SPRINTEROS loader
 * loads the kernel from SD card into memory, based on recovery 
 */
int check_sprinteros_sig(SPI* spi_master) {
    uint8_t block[512];

    if (sd_read_block(spi_master, SPI1, OS_MAX_BLOCKS-1, block)) {
        uart_out("[ SD ]: Block %d read failed", OS_MAX_BLOCKS);
        return 1;
    }
    uart_out("[ SD ]: Block %d read OK", OS_MAX_BLOCKS);

    if ((block[510] == 0x55) && (block[511] == 0xAA)) {
        uart_out("[ OS LOADER ]: Valid OS signature");
        return 0;
    }

    uart_out("[ OS LOADER ]: No OS signature (%h %h)", block[510], block[511]);
    return 1;
}

int load_sprinteros(SPI* spi_master, int recovery) {
    uint8_t *dest;

    for (int i=0; i < OS_MAX_BLOCKS; i++) {
        dest = (uint8_t *)(OS_LOAD_ADDR + ((uint32_t)i * SD_BLOCK_SIZE));

        if (sd_read_block(spi_master, SPI1, i, dest)) {
            uart_out("[ SD ]: Block %d read failed", i);
            return 1;
        }
        uart_out("Loaded SprinterOS block %d", i);

        iwdg_reset();
    }
    return 0;
}

/** 
 * SPRINTEROS BOOTLOADER
 */
int main(void) {
    if (sysclk_set_180mhz()) {
        error(1);
    }

    /* global simple timer - known by everything */
    if (init_basic_timer(TIM_6, &__global_simple_timer_ptr__)) {
        error(2);
    }

    if (uart_init(UART_COMM_PORT)) {
        error(3);
    }
    uart_out("");
    uart_out("UART initialized");
    uart_out("UART Used: %d", UART_COMM_PORT);

    /* clock verification */
#if TEST_SYSCLK
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 100; i++) {
            SysTick->LOAD = 1800000 - 1;
            SysTick->VAL = 0;
            SysTick->CTRL = 5;

            while ((SysTick->CTRL & (1 << 16)) == 0);
            SysTick->CTRL = 0;
        }
        uart_out("~1 second elapsed");
    } 
#endif

    uart_out("SprinterBoot v%s (BUILD %s)", VERSION, BUILD_DATE);
    
    /* bring up watchdog timer */
    if (iwdg_init()) {
    	uart_out("[ IWDG ]: Internal Watchdog Timer initialization Failed");
        goto loop_forever;
    }
    uart_out("[ IWDG ]: Internal Watchdog Timer initialized");
    
    iwdg_reset();

    /* ============ LOAD SPRINTER OS FROM SD CARD ============ */
    SPI* spi_master;

    if (init_spi(&spi_master, SPI1)) {
        uart_out("[ SPI ]: SPI Init Failed");
        goto loop_forever;
    } else {
        uart_out("[ SPI ]: SPI Init Successful");
    }

    iwdg_reset();

    if (sd_init(spi_master, SPI1)) {
        uart_out("[ SD ]: SD Card Init Failed");
        goto loop_forever;
    } else {
        uart_out("[ SD ]: SD Card Init Successful");
    }

    iwdg_reset();

    /* check os signature */
    if (check_sprinteros_sig(spi_master)) {
        uart_out("[ OS LOADER ]: SprinterOS signature check failed");
        goto loop_forever;
    }

    iwdg_reset();

    /* load OS */
    int boot_recovery_mode = 0;
#if RECOVERY_MDOE
    boot_recovery_mode = 0;
#endif

    if (load_sprinteros(spi_master, boot_recovery_mode)) {
        uart_out("[ OS LOADER ]: Failed to load SprinterOS");
        goto loop_forever;
    } else {
        uart_out("[ OS LOADER ]: Loaded SprinterOS into SRAM");
    }

    /* loop forever */
loop_forever:
    while (1) {
        /* testing. If timer is off, timer reset will not happen. SW reset occurs */
        iwdg_reset();
        delay_ms(800, __global_simple_timer_ptr__);
    }
}
