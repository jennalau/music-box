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
volatile unsigned int isr_freq = 0;
volatile unsigned int isr_count = 0;

void buzzer_init(void) {
    DDRB |= (1 << PB4); 	    // set buzzer output
}
void timer_init(void) {
    TCCR1B |= (1 << WGM12);     // set to CTC mode
    TIMSK1 |= (1 << OCIE1A);    // enable timer at interrupt
}
void start_timer(void) {
    TCCR1B |= (1 << CS10);      // bit for prescalar of 1
    // TCCR1B |= (0b010 << CS10);
}
void stop_timer(void) {
    TCCR1B &= ~(1 << CS10);     // bit for prescalar of 1
    // TCCR1B &= (0b111 << CS10); 
}

// play a single note from the notes array
void play_note(int note, unsigned char prescalar){
    isr_freq = note_freq[note]; // get note frequency

    // determining the timer values
    if (isr_freq == 0){
        _delay_ms(500); // delay for 0.5 seconds
        done = 1;       // reset flag
    } else {
        OCR1A = 16000000 / (2 * isr_freq * prescalar); // load max cycle count
        start_timer();
    }
    // OCR1A = 16000000 / (2 * isr_freq); // load max cycle count
    // start_timer();
}

// play each note in a tune
void play_tune(void){
    timer_init();
    // isr_count = 0;
    unsigned char prescalar = 1;

    for(int i = 0; i < 20; i++){
        int note = notes[i];
        while(!done){} // wait for previous note to finish
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
    isr_count += 1;         // keep track of times ISR is invoked

    // reached 0.5s
    if(isr_count >= isr_freq){
        isr_count = 0;
        done = 1; // done playing previous note
        stop_timer();
    } else {
        PORTB ^= (1 << PB4);    // invert PB buzzer output bit (high vs. low)
    }
}