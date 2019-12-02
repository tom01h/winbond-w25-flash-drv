# Winbond W25Q series tiny driver

This is a basic synchronous driver for the winbond W25 series of serial NOR 
flash devices. ([Datasheets](https://www.winbond.com/hq/support/resources/?__locale=en&line=/product/code-storage-flash-memory/index.html&family=/product/code-storage-flash-memory/serial-nor-flash/index.html&pno=W25Q32JV&category=/.categories/resources/datasheet/) )

It's been tested on a W25Q32JV 

## Usage

The driver is MCU agnostic, implement or replace the following SPI defines in `spi.h`):
- `SPI_EXCHANGE`: spi byte exchange - must be 8 bits wide, eg `uint8_t myfunc(uint8_t sendval)`
- `SPI_FLASH_CS_ENABLE`: of type `void()`: cs enable (drive line low) for this the flash chip slave
- `SPI_FLASH_CS_DISABLE`: of type `void()`: cs disable (drive line high) for this the flash chip slave


All W25Q32JV functions are **synchronous**, designed to be called in a RTOS system 
with tasks that can block/yield/sleep. Read/write ops spin wait on busy status to
 completion for simplicity. 


## Not implemented
Dual and quad IO ops are not implemented in this driver due to targeting 3 pin SPI.
It's not particularly fast, designed for tight / small devices over raw speed (use dual/SQI for that!)


## Misc

This driver was realized on a PIC24F08KL200 and has MPLABX + XC16 test code in main.c
