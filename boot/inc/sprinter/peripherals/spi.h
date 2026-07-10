#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stddef.h>

#include "sprinter/peripherals/gpio.h"

/* Enum for SD Response */
typedef enum {
    SD_R1,
    SD_R3,
    SD_R7
} SD_RES;

typedef enum {
    SPI0 = 0,
    SPI1 = 1,
    SPI2 = 2,
    SPI3 = 3,
    SPI4 = 4,
} SPI_NUM;

/**
 * pin mapping lookups, based on index of the SPI we're using
 *                                           SPI 1       SPI 2        SPI 3        SPI 4     
 */
static const uint16_t CS_NSS_MAPPING[] = { 0, PIN('A',4), PIN('B',12), PIN('A',15), PIN('E',11)};
static const uint16_t SCK_MAPPING[]    = { 0, PIN('A',5), PIN('B',13), PIN('B',3),  PIN('E',12)};
static const uint16_t MISO_MAPPING[]   = { 0, PIN('A',6), PIN('B',14), PIN('B',4),  PIN('E',13)};
static const uint16_t MOSI_MAPPING[]   = { 0, PIN('A',7), PIN('B',15), PIN('B',5),  PIN('E',14)};

/* SPI register structure */
typedef struct SPI {
    volatile uint32_t CR1, CR2, SR, DR, CRCPR, RXCRCR, TXCRCR;
} SPI;


int init_spi(SPI** spi_master, SPI_NUM const spi_id);
int init_sd_slave(SPI** spi_master, SPI_NUM const spi_id);

#endif
