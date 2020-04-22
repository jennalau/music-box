/*
  Code for initializing TIMER1 and its ISR (buzzer)
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "musicbox.h"

volatile unsigned char done = 1;
volatile unsigned char isr_freq = 0;

void buzzer_init(void) {
    DDRB |= (1 << PB4); 	    // set buzzer output
}
void timer_init(void) {
    TCCR1B |= (1 << WGM12);     // set fast PWM mode
    TIMSK1 |= (1 << OCIE1A);    // enable timer at interrupt, compare match interrupt
}
void start_timer(void) {
    TCCR1B |= (1 << CS10);
}
void stop_timer(void) {
    TCCR1B &= ~(1 << CS10);
}

// play a single note from the notes array
void play_note(int note, unsigned char prescalar){
    isr_freq = note_freq[note]; // get note frequency

    // determining the timer values
    OCR1A= 16000000 / (2 * isr_freq * prescalar); // load max cycle count

    start_timer();
}

// play each note in a tune
void play_tune(void){
    timer_init();
    unsigned char prescalar = 1;

    for(int i = 0; i < 21; i++){
        int note = notes[i];
        while(done == 0){} // wait for previous note to finish
        done = 0;
        play_note(note, prescalar);
  }
}

// generate an interrupt at the necessary 
// interval to flip the output bit to the speaker

// depending on note frequency, update the timer 
// each time you want to play a different frequency.

// generate interrupt every X seconds
ISR(TIMER1_COMPA_vect)
{
    // if done 
    isr_count += 1;         // keep track of times ISR is invoked
    
    // reached 0.5s
    if(isr_count == isr_freq){
        isr_count = 0;
        done = 1; // done playing previous note
        stop_timer();
    }
    
    PORTB ^= (1 << PB4);    // invert PB buzzer output bit (high vs. low)
}