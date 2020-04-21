/*
  Code for initializing TIMER1 and its ISR (buzzer)
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "musicbox.h"


volatile unsigned char isr_freq = 0;

void buzzer_init(void) {
    DDRB |= (1 << PB4); 	    // set buzzer output
}

void timer_init(void){
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
void play_note(int note){
    isr_freq = note_freq[note];
    unsigned char prescalar = 1;

    // determining the timer values
    unsigned short m = 16000000 / (2 * isr_freq * prescalar);
    OCR1A = m;                  // load max cycle count

    start_timer();
}

// play each note in a tune
void play_tune(void){
    timer_init();
  for(int i = 0; i < 21; i++){
      int note = notes[i];
      play_note(note);
      
  }
}




// generate an interrupt at the necessary 
// interval to flip the output bit to the speaker

// depending on note frequency, update the timer 
// each time you want to play a different frequency.

// generate interrupt every X seconds
ISR(TIMER1_COMPA_vect)
{
    isr_count += 1;         // keep track of how many times the ISR is invoked
    
    // reached 0.5s
    if(isr_count == isr_freq){
        stop_timer();
        isr_count = 0;
    }

    PORTB ^= (1 << PB4);    // invert PB buzzer output bit (high vs. low)

    
}