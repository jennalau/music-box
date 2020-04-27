/*
  Code for initializing TIMER2 (Pulse Width Modulation for LED)
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


// void led_init(void){
//     DDRB |= (1 << 3);  // set LED as output
//     PORTB &= ~(1 << PB3); // init the LED output to 0 (set PORTB3 as 0)

//     // WGM2[2:0] = 0x3
//     // TOP = 0xFF when WGM2[2:0] = 0x3
//       TCCR2A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00); // set to fast PWM mode
//       TIMSK2 |= (1 << TOIE2);    // enable timer at interrupt
//       OCR2A //TURN ON 
//       // set same presclar
//       TCCR2A |= (0b10 << COM2A0);  // make sure PWM is not inverted 

//     // while PWM is 1

//     // 

// }


