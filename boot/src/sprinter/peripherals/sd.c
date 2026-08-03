#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "sprinter/peripherals/sd.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals.h"

/* send 8 clock cycles (1 byte) of dummy data to SPI device */
static uint8_t send_dummy(SPI** spi) {
    while (READ_BIT((*spi)->SR, 1) == 0);
    spi_write8(*spi, 0xFF);                      /* send the dummy byte */
    while (READ_BIT((*spi)->SR, 0) == 0);        /* wait for RXNE to be not empty (means response is here) */
    return (spi_read8(*spi));                    /* read out the bits from DR and discard */
}

/**
 * send a command to SD card
 */
#define SD_CMD_PREFIX   0x40
static int sd_send_cmd(SPI** spi_master,        /* [in] SPI Master structure to set up */
                       SPI_NUM spi_id,          /* [in] SPI we're using (1->4 supported) */
                       uint8_t cmd,             /* [in] Command number we want to send */
                       uint32_t arg,            /* [in] 32 bit argument */
                       uint8_t crc,             /* [in] 8-bit CRC */
                       SD_RES response_type,    /* [in] Type of response expected */
                       uint8_t* resp_buffer     /* [out] Response buffer */
) {
    assert(spi_master != NULL);
    assert(resp_buffer != NULL);

    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ];

    /* select the slave (set chip select to low) */
    while (READ_BIT((*spi_master)->SR, 7));
    gpio_digital_write(CS_NSS_PIN, 0);
    send_dummy(spi_master);

    /* send the 6-byte command */
    uint8_t frame[6] = {
        (uint8_t)(SD_CMD_PREFIX | cmd),
        (uint8_t)(arg >> 24),
        (uint8_t)(arg >> 16),
        (uint8_t)(arg >> 8),
        (uint8_t)(arg),
        (uint8_t)(crc | 0x01),
    };

    for (int i = 0; i < 6; i++) {
        while (READ_BIT((*spi_master)->SR, 1) == 0);   /* TXE */
        spi_write8(*spi_master, frame[i]);

        while (READ_BIT((*spi_master)->SR, 0) == 0);   /* RXNE */
        (void)spi_read8(*spi_master);
    }

    /* wait for the sd card to return some results (time out after 128 bytes) */
    uint32_t timeout = 1024;

    /* R1 response type */
    while (1) {
        /* if we timed out, break out, and throw error */
        if (timeout == 0) {
            break;
        } else {
            /* spam dummy 0xFF until results received */
            uint8_t r1 = send_dummy(spi_master);
            if (r1 != 0xFF) {
                resp_buffer[0] = r1;
                break;
            }
            timeout--;
        }
    }

    /* depending on the type, possibly read more */
    if ((response_type == SD_R3) || (response_type == SD_R7)) {
        /* we're gonna get exactly 4 more bytes for response */
        for (int i = 1; i < 5; i++) {
            resp_buffer[i] = send_dummy(spi_master);
        }
    }

    /* deassert CS and send post-command clocks (8 extra cycles) */
    gpio_digital_write(CS_NSS_PIN, 1);
    send_dummy(spi_master);

    return (timeout == 0);
}

#define SD_INIT_LOOP_LIMIT  128
int init_sd_slave(SPI** spi_master, SPI_NUM const spi_id) {
    assert(spi_master != NULL);

    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ];

    /* 
     * send idle serial clock cycles while sd card wakes up 
     * just set CS to high and send 80 cycles of SCK (8 / byte, send 10 bytes)
     */
    uint8_t resp_buffer[5];

    while (READ_BIT((*spi_master)->SR, 1) == 0);
    gpio_digital_write(CS_NSS_PIN, 1);
    for (int i = 0; i < 10; i++) {
        send_dummy(spi_master);
    }

    while (READ_BIT((*spi_master)->SR, 7)); /* wait until SPI's not busy anymore to proceed */

    /* reset the card (send CMD0) */
    if (sd_send_cmd(spi_master, spi_id, 0, 0x00000000, 0x95, SD_R1, resp_buffer)) {
        uart_out("CMD0 send failed");
        return 1;
    }
    if ((resp_buffer[0] & 0x01) != 0x01) {
        return 1;
    }

    /* check SD Version (CMD8) */
    memset(resp_buffer, 0, sizeof(resp_buffer));
    if (sd_send_cmd(spi_master, spi_id, 8, 0x000001AA, 0x87, SD_R7, resp_buffer)) {
        uart_out("CMD8 send failed");
        return 1;
    }
    if (!(resp_buffer[0] & 0x01)) {
        return 1;
    }
    /* if card does not know CMD8, then SD_V1 card */
    if (((resp_buffer[0] & 0x04) >> 2) ||(resp_buffer[3] != 0x01) || (resp_buffer[4] != 0xAA)) {
        uart_out("SD_V1.x SD Card Detected");
    } else {
        uart_out("SD_V2.x SD Card Detected");
    }

    /* init SD Card (CMD55 + ACMD41 loop) */
    int loops = 0;
    memset(resp_buffer, 0, sizeof(resp_buffer));

    do {
        /* if something goes wrong, do not loop here forever, report */
        if (loops > SD_INIT_LOOP_LIMIT) {
            uart_out("CMD55 + ACMD41 loop timed out");
            return 1;
        }
        loops++;

        if (sd_send_cmd(spi_master, spi_id, 55, 0x00000000, 0x01, SD_R1, resp_buffer)) {
            uart_out("CMD55 send failed");
            return 1;
        }
        if (sd_send_cmd(spi_master, spi_id, 41, 0x40000000, 0x01, SD_R1, resp_buffer)) {
            uart_out("ACMD41 send failed");
            return 1;
        }
    } while (resp_buffer[0] != 0x00);

    /* read OCR (CMD58) to see how the card is addressed */
    memset(resp_buffer, 0, sizeof(resp_buffer));
    if (sd_send_cmd(spi_master, spi_id, 58, 0x00000000, 0x01, SD_R3, resp_buffer)) {
        uart_out("CMD58 send failed");
        return 1;
    }
    if (resp_buffer[0] != 0x00) {
        uart_out("CMD58 returned R1 error %h", resp_buffer[0]);
        return 1;
    }
    /* OCR bit 31, card still powering up */
    if (!(resp_buffer[1] & 0x80)) {
        uart_out("SD Card not ready");
        return 1;
    }
    
    if (resp_buffer[1] & 0x40) {
        uart_out("SDHC / SDXC Card (block addressed)");
    } else {
        uart_out("SDSC Card (byte addressed)");
    }

    return 0;
}
