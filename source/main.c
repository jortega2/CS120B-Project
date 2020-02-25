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
	int x, y, xy;
	unsigned char swi, up, down, left, right = 0;
	ADC_Init();
	
    while (1) {
		x = ADC_Read(0);/* Read the status on X-OUT pin using channel 0 */
		
		y = ADC_Read(1);/* Read the status on Y-OUT pin using channel 0 */

		xy = ADC_Read(2);/* Read the status on SWITCH pin using channel 0 */
		
		if (x-y == 0){
			up = 1;
			down = left = 0;
		} else if (x-y >= 700){
			down = 1;
			up = left = 0;
		} else if (x-y <=-700){
			left = 1;
			down = up = 0;
		} 
		if(y < 600)
			swi = 1;	
		else
			swi = 0;
		PORTB = swi | up << 1 | down << 2 | left << 3;	
	}
    return 1;
}
