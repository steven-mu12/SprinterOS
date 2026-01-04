/**
 ******************************************************************************
 * @file           : spi.c
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
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "sprinter/peripherals/spi.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals.h"

/* =============== HELPER FUNCTIONS ===============*/

/**
 * @brief Force 8 bit read and writes to the DR
 */
static inline void spi_write8(SPI* spi, uint8_t value) {
    *(volatile uint8_t *)&spi->DR = value;
}

static inline uint8_t spi_read8(SPI* spi) {
    return *(volatile uint8_t *)&spi->DR;
}


/**
 * @brief Send 8 clock cycles (1 byte) of dummy data to SPI device
 */
static uint8_t send_dummy(SPI** spi) {
    while (READ_BIT((*spi)->SR, 1) == 0);
    spi_write8(*spi, 0xFF);                      // send the dummy byte
    while (READ_BIT((*spi)->SR, 0) == 0);        // wait for RXNE to be not empty (means response is here)
    return (spi_read8(*spi));                    // read out the bits from DR and discard
}


/**
 * @brief Send a command to SD card
 */
static int sd_send_cmd(SPI** spi_master,        // !< [in] SPI Master structure to set up
                       SPI_NUM spi_id,          // !< [in] SPI we're using (1->4 supported)
                       uint8_t cmd,             // !< [in] Command number we want to send
                       uint32_t arg,            // !< [in] 32 bit argument
                       uint8_t crc,             // !< [in] 8-bit CRC
                       SD_RES response_type,    // !< [in] Type of response expected
                       uint8_t* resp_buffer     // !< [out] Response buffer
) {
    assert(spi_master != NULL);
    assert(resp_buffer != NULL);

    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ];

    // -- select the slave (set chip select to low)
    while (READ_BIT((*spi_master)->SR, 7));  // wait until not busy
    gpio_digital_write(CS_NSS_PIN, 0);
    send_dummy(spi_master);

    // -- send the 6 byte command in correct sequence
    uint8_t temp;
    uint16_t sent;
    for (int i = 0; i < 6; i++) {
        while (READ_BIT((*spi_master)->SR, 1) == 0);         // wait until the transmit buffer empty
        
        // send commmand based on which of the 6
        switch (i) {
            case 0:
                spi_write8(*spi_master, (0x40 | cmd));               // send command byte
                break;
            case 1:
                spi_write8(*spi_master, ((arg >> 24) & 0xFF));      // send most significant 8b of arg
                break;
            case 2:
                spi_write8(*spi_master, ((arg >> 16) & 0xFF));      // send 2nd most significant 8b of arg
                break;
            case 3:
                spi_write8(*spi_master, ((arg >> 8) & 0xFF));       // send third significant 8b of arg
                break;
            case 4:
                spi_write8(*spi_master, (arg & 0xFF));              // send least significant 8b of arg
                break;
            case 5:
                spi_write8(*spi_master, (crc | 0x01));              // send dummy crc byte
                break;
        }
        while (READ_BIT((*spi_master)->SR, 0) == 0);  // wait for RXNE to be not empty
        temp = spi_read8(*spi_master);                // read out the bits from DR, and discard
    }

    // -- wait for the sd card to return some results (time out after 128 bytes)
    uint32_t timeout = 1024; 
    temp = 0xFF;

    // - response type R1
    while (1) {
        // if we timed out, break out, and throw error
        if (timeout == 0) {         
            break;
        } else {
            // spam dummy 0xFF until results received
            temp = send_dummy(spi_master);
            if (temp != 0xFF) {
                resp_buffer[0] = temp;
                break;
            }
            timeout--;
        }
    }

    // -- depending on type, we might need to read more
    if (response_type == SD_R7) {
        // we're gonna get exactly 4 more bytes for response
        for (int i = 1; i < 5; i++) {
            temp = send_dummy(spi_master);
            resp_buffer[i] = temp;
        }
    }

    // -- deassert CS and send post-command clocks (8 extra cycles)
    gpio_digital_write(CS_NSS_PIN, 1);
    send_dummy(spi_master);

    return (timeout == 0);
}


/* =============== USER FUNCTIONS ===============*/

/**
 * @brief Init function for SPI
 */
int init_spi(
    SPI** spi_master,      // !< [in] SPI Master structure to set up
    SPI_NUM const spi_id   // !< [in] SPI we're using (1->4 supported)
) {
    assert(spi_master != NULL);

    // Telling C to treat SPIx_BASE as a SPI structure
    // Also enable RCC clock, based on spi_id (on different buses)
    if (spi_id == SPI1) { 
        *spi_master = (SPI *)(SPI1_BASE); 
        SET_BIT(RCC->APB2ENR, 12);
    }
    else if (spi_id == SPI2) { 
        *spi_master = (SPI *)(SPI2_BASE);
        SET_BIT(RCC->APB1ENR, 14);
    }
    else if (spi_id == SPI3) { 
        *spi_master = (SPI *)(SPI3_BASE);
        SET_BIT(RCC->APB1ENR, 15);
    }
    else if (spi_id == SPI4) { 
        *spi_master = (SPI *)(SPI4_BASE); 
        SET_BIT(RCC->APB2ENR, 13);
    }
    else {
        return 1;
    }

    // -- set up GPIO pins for the SPI pin
    // calculate the AF ID, it's different depending on the SPI we're using
    uint8_t spi_af_id = ((spi_id == 1) || (spi_id == 2) || (spi_id == 4)) ? 5 : 6;

    // NSS / CS
    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ];
    gpio_pinmode(CS_NSS_PIN, GPIO_MODE_OUTPUT, 0x00);
    if (gpio_digital_write_sys(CS_NSS_PIN, 1)) { return 1; }  // idle high

    // SCK
    uint16_t SCK_PIN = SCK_MAPPING[ spi_id ];
    if (gpio_pinmode(SCK_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *SCK_PIN_GPIO = GPIO_PORT_INIT(PINPORT(SCK_PIN));
    RESET_BIT(SCK_PIN_GPIO->OTYPER, PINNUM(SCK_PIN));       // push-pull mode

    // MOSI
    uint16_t MOSI_PIN = MOSI_MAPPING[ spi_id ];
    if (gpio_pinmode(MOSI_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *MOSI_PIN_GPIO = GPIO_PORT_INIT(PINPORT(MOSI_PIN));
    RESET_BIT(MOSI_PIN_GPIO->OTYPER, PINNUM(MOSI_PIN));     // push-pull mode

    // MISO
    uint16_t MISO_PIN = MISO_MAPPING[ spi_id ];
    if (gpio_pinmode(MISO_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *MISO_PIN_GPIO = GPIO_PORT_INIT(PINPORT(MISO_PIN));
    SET_BITS(MISO_PIN_GPIO->PUPDR, PINNUM(MISO_PIN)*2, 0x01, 0x03); // pull-up mode


    // -- Go through SPIx_CR1 / CR2 registers (only edited ones we needed)
    RESET_BIT((*spi_master)->CR1, 1);                       // CPOL = 0
    RESET_BIT((*spi_master)->CR1, 0);                       // CPHA = 0   
    SET_BIT((*spi_master)->CR1, 2);                         // master configuration

    if ((spi_id == SPI2) || (spi_id == SPI3)) {             // these run on APB1, which is half as fast
        SET_BITS((*spi_master)->CR1, 3, 0x06, 0x07);        // for setup, we'll be around 351 KHz (45 MHz / 128)
    } else {
        SET_BITS((*spi_master)->CR1, 3, 0x07, 0x07);        // for setup, we'll be around 351 KHz (90 MHz / 256)
    }

    SET_BIT((*spi_master)->CR1, 8);                         // software based CS, need to force HW SS to just be 1
    SET_BIT((*spi_master)->CR1, 9);                
    SET_BITS((*spi_master)->CR2, 8, 0x07, 0x0F);            // 8 bit data size
    SET_BIT((*spi_master)->CR2, 12);                        // set FRXTH to 8 bit activation

    // // CRC stuff (we'll enable this later)
    // RESET_BIT((*spi_master)->CR1, 11);                   // we'll do a 8 bit CRC
    // SET_BITS((*spi_master)->CRCPR, 0, 0x0007, 0xFFFF);   // CRC polynomial
    // SET_BIT((*spi_master)->CR1, 13);                     // enable CRC calculation
    RESET_BIT((*spi_master)->CR1, 13);                     // disable CRC calculation

    // -- Enable the SPI Peripheral
    SET_BIT((*spi_master)->CR1, 6);

    return 0;
}   


/**
 * @brief SD Initialization
 */
int init_sd_slave(
    SPI** spi_master, 
    SPI_NUM const spi_id
) {
    assert(spi_master != NULL);

    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ]; 
    uint16_t SCK_PIN = SCK_MAPPING[ spi_id ];
    uint16_t MISO_PIN = MISO_MAPPING[ spi_id ];
    uint16_t MOSI_PIN = MOSI_MAPPING[ spi_id ];

    // -- Send idle serial clock cycles while sd card wakes up
    uint8_t resp_buffer[8];

    while (READ_BIT((*spi_master)->SR, 1) == 0);            // wait until the transmit buffer is empty
    gpio_digital_write(CS_NSS_PIN, 1);                      // set chip select to high

    uint8_t temp;
    for (int i = 0; i < 10; i++) {                          // send 80 cycles of SCK (8 / byte, send 10 bytes)
        temp = send_dummy(spi_master);
    }

    while (READ_BIT((*spi_master)->SR, 7));                 // wait until SPI's not busy anymore to proceed

    // -- Reset the card (send CMD0)
    if (sd_send_cmd(spi_master, spi_id, 0, 0x00000000, 0x95, SD_R1, resp_buffer)) {
        return 1;
    }
    if ((resp_buffer[0] & 0x01) != 0x01) {
        return 1;
    }

    // -- Check SD Version (CMD8)
    bool SD_v2x = 0;

    memset(resp_buffer, 0, sizeof(resp_buffer));        // reset the buffer
    if (sd_send_cmd(spi_master, spi_id, 8, 0x000001AA, 0x00, SD_R7, resp_buffer)) {
        return 1;
    }
    if (!(resp_buffer[0] & 0x01)) {
        return 1;
    }
    if ((resp_buffer[0] & 0x04) >> 2) {
        SD_v2x = 0;
    }
    if ((resp_buffer[3] != 0x01) || (resp_buffer[4] != 0xAA)) {
        // this tells us that its an SD_V1.x card
        SD_v2x = 0;
    } else {
        SD_v2x = 1;
    }

    // -- Initialize SD Card (ACMD41 Loop)


    return 0;
}
