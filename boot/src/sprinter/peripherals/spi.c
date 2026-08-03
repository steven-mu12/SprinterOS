#include <assert.h>
#include <stdint.h>

#include "sprinter/peripherals/spi.h"

#include "sprinter/core/stm32f7.h"
#include "sprinter/peripherals.h"

/**
 * user functions
 */
int init_spi(SPI** spi_master, SPI_NUM const spi_id) {
    assert(spi_master != NULL);

    /*
     * Telling C to treat SPIx_BASE as a SPI structure
     * Also enable RCC clock, based on spi_id (on different buses)
     */
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

    /* set up the gpio pins for SPI */
    uint8_t spi_af_id = ((spi_id == 1) || (spi_id == 2) || (spi_id == 4)) ? 5 : 6;

    uint16_t CS_NSS_PIN = CS_NSS_MAPPING[ spi_id ];
    gpio_pinmode(CS_NSS_PIN, GPIO_MODE_OUTPUT, 0x00);
    if (gpio_digital_write_sys(CS_NSS_PIN, 1)) { return 1; }  /* idle high */

    uint16_t SCK_PIN = SCK_MAPPING[ spi_id ];
    if (gpio_pinmode(SCK_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *SCK_PIN_GPIO = GPIO_PORT_INIT(PINPORT(SCK_PIN));
    RESET_BIT(SCK_PIN_GPIO->OTYPER, PINNUM(SCK_PIN));       /* push-pull mode */

    uint16_t MOSI_PIN = MOSI_MAPPING[ spi_id ];
    if (gpio_pinmode(MOSI_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *MOSI_PIN_GPIO = GPIO_PORT_INIT(PINPORT(MOSI_PIN));
    RESET_BIT(MOSI_PIN_GPIO->OTYPER, PINNUM(MOSI_PIN));     /* push-pull mode */

    uint16_t MISO_PIN = MISO_MAPPING[ spi_id ];
    if (gpio_pinmode(MISO_PIN, GPIO_MODE_AF, spi_af_id)) { return 1; }
    struct gpio *MISO_PIN_GPIO = GPIO_PORT_INIT(PINPORT(MISO_PIN));
    SET_BITS(MISO_PIN_GPIO->PUPDR, PINNUM(MISO_PIN)*2, 0x01, 0x03); /* pull-up mode */


    /* Go through SPIx_CR1 / CR2 registers (only edited ones we needed) */
    RESET_BIT((*spi_master)->CR1, 1);                       /* CPOL = 0 */
    RESET_BIT((*spi_master)->CR1, 0);                       /* CPHA = 0 */
    SET_BIT((*spi_master)->CR1, 2);                         /* master configuration */

    if ((spi_id == SPI2) || (spi_id == SPI3)) {             /* these run on APB1, which is half as fast */
        SET_BITS((*spi_master)->CR1, 3, 0x06, 0x07);        /* for setup, we'll be around 351 KHz (45 MHz / 128) */
    } else {
        SET_BITS((*spi_master)->CR1, 3, 0x07, 0x07);        /* for setup, we'll be around 351 KHz (90 MHz / 256) */
    }

    SET_BIT((*spi_master)->CR1, 8);                         /* software based CS, need to force HW SS to just be 1 */
    SET_BIT((*spi_master)->CR1, 9);
    SET_BITS((*spi_master)->CR2, 8, 0x07, 0x0F);            /* 8 bit data size */
    SET_BIT((*spi_master)->CR2, 12);                        /* set FRXTH to 8 bit activation */

    /* CRC stuff (we'll enable this later)
    RESET_BIT((*spi_master)->CR1, 11);                      -- we'll do a 8 bit CRC
    SET_BITS((*spi_master)->CRCPR, 0, 0x0007, 0xFFFF);      -- CRC polynomial
    SET_BIT((*spi_master)->CR1, 13);                        -- enable CRC calculation */
    RESET_BIT((*spi_master)->CR1, 13);                     /* disable CRC calculation */

    /* -- Enable the SPI Peripheral */
    SET_BIT((*spi_master)->CR1, 6);

    return 0;
}
