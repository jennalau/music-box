/*
  Code for initializing TIMER2 (Pulse Width Modulation for LED)
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "musicbox.h"


void led_init(void){
    // set LED as output
    DDRB |= (1 << 3);  

    // set to fast PWM mode , WGM2[2:0] = 0x3
    TCCR2A |= ((1 << WGM21) | (1 << WGM20)); 

    // enable timer at interrupt
    TIMSK2 |= (1 << TOIE2);     // issue here?

    // make sure PWM is not inverted 
    // Clear OC2A on Compare Match, set OC2A at BOTTOM (non-inverting mode)
    TCCR2A |= (1 << COM2A1);
    TCCR2A &= ~(1 << COM2A0); 

    // set brightness (0 - 255)
    OCR2A = 255;
    
    // start timer
    TCCR2B |= (1 << CS21); // prescalar of 1
}


ISR(TIMER2_OVF_vect){
  OCR2A = pwm_width;  //set OCR2A (pwm width)
}
