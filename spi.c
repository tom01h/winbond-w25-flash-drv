#include "spi.h"


uint8_t spi_exchange(uint8_t ex)
{
    return 0;
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

