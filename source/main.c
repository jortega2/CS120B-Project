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
#include <stdio.h>
#include "adc.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	int x, y;
	unsigned char center, up, down, left, right;
	up = down = left = right = 0;
	ADC_Init();
	
    while (1) {
		x = ADC_Read(0);/* Read the status on X-OUT pin using channel 0 */
		
		y = ADC_Read(1);/* Read the status on Y-OUT pin using channel 0 */

			
		if ((x> 400 && x < 600)&&(y > 400 && y < 600)){
			center = 0x10;
			down = left = right = up = 0;
		} else if ((y > 900) && (x > 400 && x < 600)){
			up = 0x01;
			down = center  = left = right = 0;
		} else if ((y < 100) && (x > 400 && x < 600)){
			down = 0x08;
			center = up = left = right = 0;
		} else if ((x < 100) && (y > 400 && y < 600)) {
			left = 0x04;
			center = up = down = right = 0;
		} else if ((x > 900) && (y > 400 && y < 600)){
			right  = 0x02;
			center = up = down = left = 0;
		}

		PORTD = up | right  | left | down | center;
	}
    return 1;
}
