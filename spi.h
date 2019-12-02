#ifndef W25_FLASH_SPI_H
#define W25_FLASH_SPI_H

#include "xc.h"

uint8_t spi_exchange(uint8_t ex);
void spi_flash_cs_en(void);
void spi_flash_cs_dis(void);

void spi_enable_PIC24F16KL402x();

#define SPI_EXCHANGE spi_exchange
#define SPI_FLASH_CS_ENABLE spi_flash_cs_en
#define SPI_FLASH_CS_DISABLE spi_flash_cs_dis



#endif