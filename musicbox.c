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
#include "buzzer.h"
#include "led.h"

#define ADC_CHANNEL 0
#define left_adc 156
#define down_adc 104
#define right_adc 0
#define up_adc 52
#define slct_adc 205

#define NUM_TONES 26
#define NUM_NOTES 21

// display definitions
unsigned char curr_screen = 1;
unsigned char next_screen = 1;
unsigned char scr_changed = 0;
int curr_col = 1;

// buzzer definitions


//rotary encoder definitions
volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for encoder state change
volatile unsigned int count = 0;		// count of note_str index
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
unsigned char notes[21] = {17, 17, 18, 20, 20, 
                          18, 17, 15, 13, 13,
                          15, 17, 17, 15, 15, 
                          0,  0,  0,  0,  0};
// unsigned char notes[NUM_NOTES] = {17, 0, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15};

// array of notes, with the index mapping to the note & frequency alike
char *note_str[] = {"  ", "C ","C#", "D ","D#", "E ","F ", "F#",
                    "G ", "G#","A ", "A#","B ", "C ","C#", "D ",
                    "D#", "E ","F ", "F#","G ", "G#","A ", "A#",
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
  // Initialize various modules and functions
  lcd_init();
  adc_init();
  encoder_init();
  buzzer_init();
  // led_init();
  sei();                //enable global interrupts

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

  // check if button is held down
  unsigned char btn_down = 0;


  // if(!btn_down) { 
  //   //TODO: Read tune from EEPROM 
  //   unsigned char *eeprom_data[21];
  //   int num_bytes = 21;
  //   int start_addr = 0;
  //   eeprom_read_block(eeprom_data, (void *) start_addr, num_bytes);

  //   //TODO: Check values read in are valid (0-25)
  //   unsigned eeprom_valid = 1;
  //   for(int i = 0; i < 21; i++){
  //     unsigned char d = eeprom_data[i];
  //     if(d > 25){
  //       eeprom_valid = 0;
  //       break;
  //     }
  //   }

  //   // use eeprom data
  //   if(eeprom_valid){
  //     notes = eeprom_data;
  //   }

  // }

  // use 'notes' as default tune if btn_down or !eeprom_valid

  while (1) {    
    lcd_moveto(0, curr_col);  // update user's cursor

    // display new screen
    if(scr_changed){
      show_screen();
      scr_changed = 0; // reset flag
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
          curr_col    = 13;
        } else if (curr_screen == 3){
          scr_changed = 1;
          next_screen = 2;
          curr_col    = 13;
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
          curr_col    = 1;
        } else if (curr_screen == 2){
          scr_changed = 1;
          next_screen = 3;
          curr_col    = 1;
        }
      }
    }

    // select button pressed
    else if (slct_press)
    {
      // TODO: write/save tune to EEPROM (non-volatile memory)
      // write_tune(); 

      // play tune via buzzer
      play_tune();
    }
    
    /* If rotary encoder was rotated, change note tone */
    if(changed) {
      changed = 0;	// Reset changed flag
      update_note();
      // update_octave(); TODO
      lcd_moveto(0, curr_col);
    }
    curr_screen = next_screen;
  }
  while (1) {} // Loop forever
}
