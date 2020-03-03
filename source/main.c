/*	Author: jorte057
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "adc.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "N5110.h"
#include "SPI_Master_H_file.h"
#include <avr/interrupt.h>
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms. 
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks 
unsigned char threeLEDs;
unsigned char blinkingLED;
unsigned char speaker;

void TimerOn(){

TCCR1B = 0x0B;

OCR1A = 125; 

TIMSK1 = 0x02;

TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M;

SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00; // bit3bit1bit0: timer off

}

void TimerISR(){
	TimerFlag = 1;
}

void N5110_Cmnd(char DATA)
{
	PORTB &= ~(1<<DC);  /* make DC pin to logic zero for command operation */
	SPI_SS_Enable();  /* enable SS pin to slave selection */	
	SPI_Write(DATA);  /* send data on data register */
	PORTB |= (1<<DC);  /* make DC pin to logic high for data operation */
	SPI_SS_Disable();
}

void N5110_Data(char *DATA)
{
	PORTB |= (1<<DC);  /* make DC pin to logic high for data operation */
	SPI_SS_Enable();  /* enable SS pin to slave selection */
	int lenan = strlen(DATA);  /* measure the length of data */
	int g =0;
	for (g=0; g<lenan; g++)
	{
		int index = 0;
		for (index = 0; index<5; index++)
		{
			SPI_Write(ASCII[DATA[g] - 0x20][index]);  /* send the data on data register */			
		}
		SPI_Write(0x00);
	}							
	SPI_SS_Disable();									
}

void N5110_Reset()  /* reset the Display at the beginning of initialization */
{
	PORTB &= ~(1<<RST);
	_delay_ms(100);
	PORTB |= (1<<RST);
}

void N5110_init()
{
	N5110_Reset();  /* reset the display */
	N5110_Cmnd(0x21);  /* command set in addition mode */
	N5110_Cmnd(0xC0);  /* set the voltage by sending C0 means VOP = 5V */
	N5110_Cmnd(0x07);  /* set the temp. coefficient to 3 */
	N5110_Cmnd(0x13);  /* set value of Voltage Bias System */
	N5110_Cmnd(0x20);  /* command set in basic mode */
	N5110_Cmnd(0x0C);  /* display result in normal mode */
}

void lcd_setXY(char x, char y)  /* set the column and row */
{
	N5110_Cmnd(x);
	N5110_Cmnd(y);
}

void N5110_clear()  /* clear the Display */
{
	SPI_SS_Enable();
	PORTB |= (1<<DC);
	int k = 0;
	for (k=0; k<=503; k++)
	{
		SPI_Write(0x00);		
	}
	PORTB &= ~(1<<DC);
	SPI_SS_Disable();	
}

void N5110_image(const unsigned char *image_data)  /* clear the Display */
{
	SPI_SS_Enable();
	PORTB |= (1<<DC);
	int k  = 0;
	for (k=0; k<=503; k++)
	{
		SPI_Write(image_data[k]);
	}
	PORTB &= ~(1<<DC);
	SPI_SS_Disable();
}

void writeCenter(){
	N5110_Data("center");
}
void writeLeft(){
	N5110_Data("left");
}
void writeRight(){
	N5110_Data("right");
}
void writeDown(){
	N5110_Data("down");
}
void writeUp(){
	N5110_Data("up");
}


int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	int x, y;
	unsigned char center, up, down, left, right;
	center = up = down = left = right = 0;
	ADC_Init();
	SPI_Init();
	N5110_init();
	N5110_clear();
	lcd_setXY(0x40, 0x80);
    while (1) {
		
		x = ADC_Read(0);/* Read the status on X-OUT pin using channel 0 */
		
		y = ADC_Read(1);/* Read the status on Y-OUT pin using channel 0 */

			
		if ((x> 400 && x < 600)&&(y > 400 && y < 600)){
			writeCenter();
			center = 0x10;
			down = left = right = up = 0;
		} else if ((y > 900) && (x > 400 && x < 600)){
			writeUp();
			up = 0x01;
			down = center  = left = right = 0;
		} else if ((y < 100) && (x > 400 && x < 600)){
			writeDown();
			down = 0x08;
			center = up = left = right = 0;
		} else if ((x < 100) && (y > 400 && y < 600)) {
			writeLeft();
			left = 0x04;
			center = up = down = right = 0;
		} else if ((x > 900) && (y > 400 && y < 600)){
			writeRight();
			right  = 0x02;
			center = up = down = left = 0;
		}

		PORTD = up | right  | left | down | center;
	}
    return 1;
}
