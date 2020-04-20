/*
  Code for rotary encoder
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "encoder.h"

#define ENCODER_BITS ((1 << PC1) | (1 << PC5))


void encoder_init(void) {
	// encoder
  	PORTC |= ENCODER_BITS;	// enable pull up resistors
	PCICR |= (1 << PCIE1); 						// set PCIE1 bit for port C
	PCMSK1 |= ((1 << PCINT9) | (1 << PCINT13));	// set PCINT9 (PC1) & PCINT13 (PC5) in PORTC mask register
}

// interrupts for PORTC
ISR(PCINT1_vect) {
	// Read the input bits and determine A and B
	unsigned char in = PINC;
	a = in & (1 << PC1);
	b = in & (1 << PC5);

	// For each state, examine the two input bits to see if state
	// has changed, and if so set "new_state" to the new state,
	// and adjust the count value.
	if (old_state == 0) {
		// Handle A and B inputs for state 0
		if(a){ // a = 1
			new_state = 1;
      if(count < 25)
        count += 1;
		} else if (b) { // b = 1
			new_state = 2; 
			if(count > 0)
				count -= 1;
		}
	}
	else if (old_state == 1) {
		// Handle A and B inputs for state 1
		if(!a){
			new_state = 0; 
			if(count > 0)
				count -= 1;
		} else if (b) {
			new_state = 3;
			if(count < 25)
        count += 1;
		}
	}
	else if (old_state == 2) {
		// Handle A and B inputs for state 2
		if(a){
			new_state = 3;
			if(count > 0)
				count -= 1;
		} else if (!b){
			new_state = 0;
			if(count < 25)
        count += 1;
		}
	}
	else {   // old_state = 3
		// Handle A and B inputs for state 3
		if(!a){
			new_state = 2;
			if(count < 25)
        count += 1;
		} else if (!b){
			new_state = 1;
			if(count > 0)
				count -= 1;
		}
	}

	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
		changed = 1;
		old_state = new_state;
	}
}