#include "spi.h"


void spi_enable_PIC24F16KL402x() {  
    /* set sclk1, sdo1 outputs*/
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;    
    /* set sdi1 input */
    TRISBbits.TRISB14 = 1;
    
    /* set RB15 as our SS gpio */
    TRISBbits.TRISB15 = 0;
    
    /* enable spi*/
    SSP1CON1bits.WCOL = 0; /* disable write collision bit */
    SSP1CON1bits.SSPEN = 1; /* enable as SPI port */
    SSP1CON1bits.SSPM = 1; /* master, Fosc / 8 = 1MHZ internal  */    
    SSP1CON3bits.BOEN = 0; /* disable buffer overflow en bit (slave only) */
    
    SSP1STATbits.CKE = 1;  /* 1 = transmit active->idle, = idle->active */
    SSP1CON1bits.CKP = 0;  /* clock polarity, 1 = active high */
    // SSP1STATbits.SMP = 0; /* 1 = sample at end, 0 = sample middle */
}

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

