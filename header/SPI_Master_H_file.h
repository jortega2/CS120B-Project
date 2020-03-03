/*
 * SPI_Master_H_file.h
 *
 * http://www.electronicwings.com
 */ 


#ifndef SPI_MASTER_H_FILE_H_
#define SPI_MASTER_H_FILE_H_

#include <avr/io.h>

#define MOSI 5
#define MISO 6
#define SCK 7
#define SS 4
#define DC 1
#define RST 0

void SPI_SS_Enable(){
        PORTB &= ~(1<<SS);
}

void SPI_SS_Disable(){
        PORTB |= (1<<SS);
}

void SPI_Init(){
        DDRB = (1<<MOSI)|(1<<SCK)|(1<<SS)|(1<<RST)|(1<<DC);                             /* Set MOSI and SCK output, all others input */
        PORTB |= (1<<RST);                                                                                              /* define reset pin as a output */
        DDRB &= ~(1<<MISO);                                                                                             /* define MISO pin as a input */
        PORTB |= (1<<SS);                                                                                               /* Make SS pin as a output */
        SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);                                                    /* Enable SPE, MSTR and SPR0 bit in SPI control register */
}

void SPI_Write(char write_data){
	SPDR = write_data;
        while(!(SPSR & (1<<SPIF)));                                                                             /* wait here while SPIF flag set */

}

char SPI_Read(){
	SPDR = 0xFF;
        while(!(SPSR & (1<<SPIF)));                                                                             /* wait here while SPIF flag set */
        return(SPDR);                                                                                                   /* Return the SPDR value */
}



#endif /* SPI_MASTER_H_FILE_H_ */
