/********************************************
 *
 *  Name: Jenna Lau
 *  Email: jennanla@usc.edu
 *  Section: Friday 11AM
 *  Assignment: Project - Music Box
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "lcd.h"
#include "adc.h"
#include "encoder.h"
#include "display.h"

#define ADC_CHANNEL 0
#define left_adc 156
#define down_adc 104
#define right_adc 0
#define up_adc 52
#define slct_adc 205

#define NUM_TONES 26
#define NUM_NOTES 21

unsigned char curr_screen = 1;
unsigned char scr_changed = 0;

volatile unsigned char isr_count = 0; 

//rotary encoder definitions
volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile unsigned int count = 0;		// Count to display
volatile unsigned char a, b;



/* Some sample tunes for testing */
/*
  The "notes" array is used to store the music that will be played.  Each value
  is an index into the tone value in the note_freq array.
*/

// G, A, F, F(octive lower), C     Close Encounters
//unsigned char notes[NUM_NOTES] = {20, 22, 18, 6, 13};

// D D B C A B G F# E D A C A B    Fight On
//unsigned char notes[NUM_NOTES] = {15, 15, 12, 13, 10, 12, 8, 7, 5, 3, 10, 13, 10, 12};

// E E F G G F E D C C D E E D D   Ode to Joy
unsigned char notes[21] = {17, 17, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15};
// unsigned char notes[NUM_NOTES] = {17, 0, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15};

// array of notes, with the index mapping to the note & frequency alike
char *note_str[] = {"  ", "C ",
                    "C#", "D ",
                    "D#", "E ",
                    "F ", "F#",
                    "G ", "G#",
                    "A ", "A#",
                    "B ", "C ",
                    "C#", "D ",
                    "D#", "E ",
                    "F ", "F#",
                    "G ", "G#",
                    "A ", "A#",
                    "B ", "C "};
/*
  The note_freq array contains the frequencies of the notes from C3 to C5 in
  array locations 1 to 25.  Location 0 is a zero for rest(?) note.  Used
  to calculate the timer delays.
*/
unsigned int note_freq[NUM_TONES] = {0, 131, 139, 147, 156, 165, 176, 185, 196, 208, 220, 233, 247,
                                     262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};


int main(void)
{

  // read from EEPROM, make sure it's valid, else use 'notes' default
  // ?
  // ?

  // Initialize various modules and functions
  lcd_init();
  adc_init();
  encoder_init();
  sei(); 										//enable global interrupts


  int curr_col = 1;
  int next_screen = 1; 

  splash_screen();      // Show splash screen for 1 second
  lcd_writecommand(1);  // clear the screen

  show_screen();        // display arrows & notes/octaves on LCD

  // Read initial state of rotary encoder
	unsigned char input = PINC;
	a = input & (1 << PC1);
	b = input & (1 << PC5);

    if (!b && !a)
		  old_state = 0;
    else if (!b && a)
		  old_state = 1;
    else if (b && !a)
		  old_state = 2;
    else
		  old_state = 3;
    new_state = old_state;

  while (1)
  {    
    lcd_moveto(0, curr_col);  // update user's cursor

    // display new screen
    if(scr_changed){
      show_screen();
      scr_changed = 0;
    }
      
    // display changes from rotary encoder
    // TODO: THIS IS NOT SHOWING IDX?
    if(changed) {
      changed = 0;	// Reset changed flag
			
      // Output count to LCD
      lcd_moveto(0, curr_col);
      _delay_ms(100);
      lcd_stringout(note_str[count]);
      lcd_moveto(0, curr_col);
    }
    
    /* Check if a button on the LCD was pressed */
    unsigned char input = adc_sample(ADC_CHANNEL);
    int left_down_press = check_button_press(input, left_adc, down_adc);
    int right_up_press  = check_button_press(input, right_adc, up_adc);
    int slct_press      = check_button_press(input, slct_adc, -1000);

    // left or down button pressed
    if (left_down_press){
      // move left
      if(curr_col > 1){
        curr_col -= 2;
        lcd_moveto(0, curr_col);
      // go to prev screen
      } else if (curr_col == 1){
        // update screen & current column states
        if(curr_screen == 2){
          scr_changed = 1;
          next_screen = 1;
          curr_col = 13;
        } else if (curr_screen == 3){
          scr_changed = 1;
          next_screen = 2;
          curr_col = 13;
        }
      }
    }
    
    // right or up button pressed
    else if (right_up_press)
    {
      // move right
      if(curr_col < 13){
        curr_col += 2;
        lcd_moveto(0, curr_col);
      // go to next screen
      } else if (curr_col == 13){
        // update screen & current column states
        if(curr_screen == 1){
          scr_changed = 1;
          next_screen = 2;
          curr_col = 1;
        } else if (curr_screen == 2){
          scr_changed = 1;
          next_screen = 3;
          curr_col = 1;
        }
      }
    }

    // select button pressed
    // else if (slct_press)
    // {
    //   // write/save tune to EEPROM (non-volatile memory)

    //   // play tune via buzzer
   
      
    // }

    curr_screen = next_screen;

    /* If rotary encoder was rotated, change note tone */


  }

  // while (1)
  // { // Loop forever
  // }
}

/* ------------------------------------------------------------------ */



/* ------------------------------------------------------------------ */

/*
  Code for initializing TIMER1 and its ISR (buzzer)
*/

// play each note in a tune
void play_tune(){

  // // play each note in the tune
  // for(int i = 0; i < NUM_NOTES; i++){
  //   int note = notes[i];
  //   int freq = note_freq[note];
  //   int time = 1 / freq;

  //   // update timer 
  //   timer_init();

    

  //   // play frequency

  // }
  
}

void timer_init(unsigned short m){
    TCCR1B |= (1 << WGM12);     // set fast PWM mode
    // WGMx[2:0] bits for Fast PWM

    TIMSK1 |= (1 << OCIE1A);    // enable timer at interrupt, compare match interrupt
    OCR1A = m;                  // load max cycle count
    TCCR1B |= (1 << CS12);      // set prescalar & start counter
}

void start_timer() {
    TCCR1B |= (1 << CS10) | (1 << CS11);
}
void stop_timer() {
    TCCR1B &= ~((1 << CS10) | (1 << CS11));
}

// generate an interrupt at the necessary 
// interval to flip the output bit to the speaker

// depending on note frequency, update the timer 
// each time you want to play a different frequency.

// generate interrupt every X seconds
ISR(TIMER1_COMPA_vect)
{
  isr_count += 1; // keep track of how many times the ISR is invoked

  // interrupt every 0.5s
  if(isr_count == 500){

    isr_count = 0;
  }
  //  determining the timer values
  // 50% duty cycle square wave at the frequency required

  // invert output bit?
  // WGMx[2:0] bits f

  // OC1A is the name of the output signal which goes to one of the pins of the IC.  
  // OCR1A is the name of a 16-bit register in the TIMER1 module.



}

/* ------------------------------------------------------------------ */

/*
  Code for initializing TIMER2 (Pulse Width Modulation for LED)
*/


/* ------------------------------------------------------------------ */

