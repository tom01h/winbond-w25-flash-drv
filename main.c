#pragma config FNOSC = FRC
#pragma config ICS = PGx2

#include "xc.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "w25_flash.h"
#include "spi.h"

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
    
    char write_buf[] = { 0x01, 0x02, 0x03, 0x04 };
    char read_buf[4] = { 0 };    
              
    while (1) {
        delay();        
        delay();
        
        w25_flash_write_enable();
        w25_flash_chip_erase();
             
        /* ensure we get 0xff (erased) */
        uint8_t tb = 0;
        w25_flash_read_data(0, &tb, 1);        
        
        /* now write some data and read it back */
        w25_flash_write_enable();
        w25_flash_page_program(0, write_buf, 4, false);
        
        w25_flash_read_data(0, read_buf, 4);
        
        bool success = tb == 0xFF && read_buf[0] == write_buf[0] 
                && read_buf[1] == write_buf[1]
                && read_buf[2] == write_buf[2]
                && read_buf[3] == write_buf[3];
        
        /* NOTE: for some reason RA4 and RB4 wont work as output, couldnt find periph */           
        //uint8_t device_id = w25_flash_read_device_id();
        /* sink pulled high */
        PORTAbits.RA2 = success ? 0 : 1;
        
        
        delay();
        delay();
    }
    
    return 0;
}
