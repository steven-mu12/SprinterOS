#ifndef __SD_H__
#define __SD_H__

#include <stdint.h>

#include "sprinter/peripherals/spi.h"

/* sd response types (tells how much data is incoming) */
typedef enum {
    SD_R1,
    SD_R3,
    SD_R7
} SD_RES;

int init_sd_slave(SPI** spi_master, SPI_NUM const spi_id);

#endif
