#ifndef __SD_H__
#define __SD_H__

#include <stdint.h>

#include "sprinter/peripherals/spi.h"

#define SD_BLOCK_SIZE   512

/* sd response types (tells how much data is incoming) */
typedef enum {
    SD_R1,
    SD_R3,
    SD_R7
} SD_RES;

int sd_init(SPI* spi_master, SPI_NUM const spi_id);
int sd_read_block(SPI* spi_master, SPI_NUM spi_id, uint32_t block, uint8_t* resp_buffer);

#endif
