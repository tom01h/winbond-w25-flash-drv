#include "spi.h"


uint8_t spi_exchange(uint8_t v) {
    /* ensure empty first. this is needed on some PIC devices with no FIFO */
//    uint8_t dummy = SSP1BUF;
//    while (SSP1STATbits.BF == 1) {
//        spi_wait();
//    }
    
    SSP1BUF = v;
    
    while (SSP1STATbits.BF == 0) {        
    }
    
    uint8_t recv = SSP1BUF;
    return recv;
}

/* pull CS/SS low - RD15*/
void spi_flash_cs_en(void)
{
    PORTBbits.RB15 = 0;
}

void spi_flash_cs_dis(void)
{
    PORTBbits.RB15 = 1;
}

