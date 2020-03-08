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
#include <avr/interrupt.h>
#include "images.h"
#endif
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms. 
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks 
unsigned char score;
char buffer[5];
short cnt, symbol;

int x, y;

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

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum screens {s_init, off, wait, countdown,  play, gameOver, gameOverWait} state;

void TickFct_screen(){
		switch (state){
			case s_init:
				state = off;
				cnt = 0;
				score = 0;
				lcd_setXY(0x50, 0x05);
				N5110_Data("off");
				break;
			case off:
				PORTD = 0x01;	
				cnt++;
				if ((~PINA & 0x08) == 0x08){
					//PORTD = 0x02;
					symbol = cnt%4;
					cnt = 0;
					state = countdown;
				} else {
					state = off;
				}	
				break;
			case wait:
				PORTD = 0x0F;
				cnt++;
				N5110_clear();
				if (cnt > 50){
                                        if (symbol == 0){
                                                //N5110_clear();
                                                lcd_setXY(0x40, 0x80);
                                                //N5110_Data("up wait");
						N5110_image(up);
                                        } else if (symbol == 1){
                                                //N5110_clear();
                                                lcd_setXY(0x40, 0x80);
                                                //N5110_Data("left wait");
						N5110_image(left);
                                        } else if (symbol == 2){
                                                //N5110_clear();
                                                lcd_setXY(0x40, 0x80);
                                                //N5110_Data("right wait");
						N5110_image(right);
                                        } else if (symbol == 3){
                                                //N5110_clear();
                                                lcd_setXY(0x40, 0x80);
                                                //N5110_Data("down wait");
						N5110_image(down);
                                        } else {
						lcd_setXY(0x40, 0x80);
						N5110_Data("defaulted...");
					}
					cnt = 0;
					state = play;
				} else {
					state = wait;
				}
				break;
			case countdown:
				PORTD = 0x03;
				if (cnt == 0){	
					N5110_clear();
					N5110_image(three);
				} else if (cnt == 1000){
					N5110_clear();
					N5110_image(two);
				} else if (cnt == 2000){
					N5110_clear();
					N5110_image(one);
				}
				if (cnt < 3000){
					state = countdown;
				} else {
					cnt = 0;
					state = wait;
				}
				cnt++;
				break;
			case play:
				PORTD = 0x04;
				//cnt++;
				lcd_setXY(0x40, 0x80);
				//N5110_Data(cnt);
				if (cnt >= 600){
					//PORTD = 0xFF;
					cnt = 0;
					state = gameOver;
				} else if ((y > 900) && (x > 400 && x < 600)){
					if (symbol == 0){
                                                symbol = cnt%4;
                                                cnt = 0;
                                                score++;
						state = wait;
                                        } else {
						symbol = cnt%4;
                                                state = gameOver;
                                        }
				} else if ((y < 100) && (x > 400 && x < 600)){
					if (symbol == 3){
                                                symbol = cnt%4;
                                                cnt = 0;
                                                score++;
						state = wait;
                                        } else {
						symbol = cnt%4;
                                                state = gameOver;
                                        }
				} else if ((x < 100) && (y > 400 && y < 600)) {
					if (symbol == 2){
                                                symbol = cnt%4;
                                                cnt = 0;
                                                score++;
						state = wait;
                                        } else {
						symbol = cnt%4;
                                                state = gameOver;
                                        }
				} else if ((x > 900) && (y > 400 && y < 600)){
					if (symbol == 1){
                                                symbol = cnt%4;
                                                cnt = 0;
                                                score++;
						state = wait;
                                        } else {
						symbol = cnt%4;
                                                state = gameOver;
                                        }
				} else {
					cnt++;
					state = play;
				}
				break;
			case gameOver:
				N5110_clear();
				lcd_setXY(0x01, 0x05);
				N5110_Data("gameOver      Score:");
				sprintf(buffer, "%d", score);
				N5110_Data(buffer);
				N5110_Data("       Press button 1 to play again");
				state = gameOverWait;
				break;
			case gameOverWait:
				//PORTD = 0x08;
				if ((~PINA & 0x08) == 0x08){
					state = s_init;
				} else { 
					state = gameOverWait;
				}
				break;
			default:
				break;
		}
}

enum joystick_states {j_init, j_main} js_state;

void TickFct_joystick(){
	switch (js_state){
		case j_init:
			x = 0;
			y = 0;
			js_state = j_main;
			break;
		case j_main:
			x = ADC_Read(0);/* Read the status on X-OUT pin using channel 0 */
			y = ADC_Read(1);/* Read the status on Y-OUT pin using channel 0 */
			js_state = js_state;
			break;
		default:
			break;
	}
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	state = s_init;
	js_state = j_init;
	ADC_Init();
	SPI_Init();
	N5110_init();
	N5110_clear();
	lcd_setXY(0x50, 0x05);
	//unsigned long N_elapsedTime = 100;
	TimerSet(1);
	TimerOn();
    while (1) {
		TickFct_joystick();
		//if (N_elapsedTime == 100){
			TickFct_screen();
			//N_elapsedTime = 0;
		//}
		while (!TimerFlag){}
		TimerFlag = 0;
		//N_elapsedTime += 1;
		
	}
    return 1;
}
