/**
 ******************************************************************************
 * @file           : spi.h
 * @author         : Steven Mu
 * @summary		   : Bootloader SPI Driver
 ******************************************************************************
 * MIT License

 * Copyright (c) 2025 Steven Mu

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 ******************************************************************************
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stddef.h>

#include "sprinter/peripherals/gpio.h"

/**
 * @brief SD Utility
 */

/* Enum for SD Response */
typedef enum {
    SD_R1,
    SD_R3,
    SD_R7
} SD_RES;

/**
 * @brief SPI Register Structure
 */

/* Enum for SPIs we got */
typedef enum {
    SPI0 = 0,
    SPI1 = 1,
    SPI2 = 2,
    SPI3 = 3,
    SPI4 = 4,
} SPI_NUM;

/* Pin mapping lookups, based on index of the SPI we're using */
//                                            SPI 1       SPI 2        SPI 3        SPI 4
static const uint16_t CS_NSS_MAPPING[] = { 0, PIN('A',4), PIN('B',12), PIN('A',15), PIN('E',11)};
static const uint16_t SCK_MAPPING[]    = { 0, PIN('A',5), PIN('B',13), PIN('B',3),  PIN('E',12)};
static const uint16_t MISO_MAPPING[]   = { 0, PIN('A',6), PIN('B',14), PIN('B',4),  PIN('E',13)};
static const uint16_t MOSI_MAPPING[]   = { 0, PIN('A',7), PIN('B',15), PIN('B',5),  PIN('E',14)};

/* SPI register structure */
typedef struct SPI {
    volatile uint32_t CR1, CR2, SR, DR, CRCPR, RXCRCR, TXCRCR;
} SPI;

/**
 * @brief Init function for SPI
 */
int init_spi(SPI** spi_master, SPI_NUM const spi_id);

/**
 * @brief SD Initialization
 */
int init_sd_slave(SPI** spi_master, SPI_NUM const spi_id);

/**
 * @brief SD Initialization
 */
int sd_read_block();

#endif
