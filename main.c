#pragma config FNOSC = FRC
#pragma config ICS = PGx2

#include "xc.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>


#include "w25_flash.h"


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

/* on p24F08KL200 this ends up around ~95ms on internal Fosc */
void delay() {
    uint16_t i = 0;
    for (i = 0; i < 64000; ++i) {
    }
}

int main(void) {
    TRISA = 0x0;
    TRISB = 0x0;
    ODCA = 0x0;
    ODCB = 0x0;
    ANSA = 0x0;
    
    /* important for PIC24F -
     *  disable analog on port b or SPI reads are always 0x00 */
    ANSB = 0x0; 
    
    spi_enable_PIC24F16KL402x();
        
    while (1) {        
        delay();        
        delay();
        
        uint8_t device_id = w25_flash_read_device_id();
        
        PORTAbits.RA4 = device_id == 0x15 ? 1 : 0;        
        
        delay();
        delay();
    }
    
    return 0;
}
