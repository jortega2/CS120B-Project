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
	int x, y, xy, t;
	unsigned char center, up, down, left, right = 0;
	ADC_Init();
	
    while (1) {
		x = ADC_Read(0);/* Read the status on X-OUT pin using channel 0 */
		
		y = ADC_Read(1);/* Read the status on Y-OUT pin using channel 0 */

		xy = ADC_Read(2);/* Read the status on SWITCH pin using channel 0 */
			
		if ((x> 400 && x < 600)&&(y > 400 && y < 600)){
			center = 1;
			down = left = right = up = 0;
		}else {
			center  = 0;
		}/* else if (){
			up = 1;
			down = center  = left = right = 0;
		} else if (){
			down = 1;
			center = up = left = right = 0;
		} else if () {
			left = 1;
			center = up = down = right = 0;
		} else if (){
			right = 1;
			center = up = down = left = 0;
		}*/

		PORTD = up;
		PORTB = center;
	}
    return 1;
}
